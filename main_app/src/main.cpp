#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "shader_util.h"
#include "RadarGeometry.h"
#include "RadarRenderer.h"
#include "udp_listener.h"
#include "text_vertex.h"

UdpListener listener(5555);
struct RadarState
{
    float sweepAngle = 0.0f;
};

void processInput(GLFWwindow *window);

void drawRadar(void *userData)
{
    if (glGetError() != GL_NO_ERROR)
        std::cout << "GL error before drawing\n";

    auto *state = static_cast<RadarState *>(userData);
    static RadarGeometry geo(0.0f, 0.0f, 60.0f);
    static RadarRenderer gridRenderer, sweepRenderer;

    if (gridRenderer.getVertexCount() == 0)
    {
        auto gridVert = geo.generateGrid(5, 12);
        gridRenderer.upload(gridVert);
    }

    auto sweepvert = geo.generateStoppedSweep(state->sweepAngle);
    sweepRenderer.upload(sweepvert);

    gridRenderer.render(GL_LINES);
    sweepRenderer.render(GL_TRIANGLE_FAN);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        std::cout << "GL error: " << std::hex << err << std::dec << "\n";
}

void drawText(void *data)
{
    static std::vector<std::string> displayLines;
    static std::mutex displayMutex;
    static TextVertex textBuilder;
    static RadarRenderer renderer;

    // Pull messages from UDP listener
    std::string msg;
    while (listener.popMessage(msg))
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        displayLines.push_back(msg);
        if (displayLines.size() > 20)
            displayLines.erase(displayLines.begin());
    }

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    int screenW = vp[2];
    int screenH = vp[3];

    std::vector<RadarVertex> vertices;
    float x = 10.0f, y = 20.0f;
    float scale = 2.0f;
    const float lineSpacing = 15.0f;
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        for (const auto &line : displayLines)
        {
            auto lineVerts = textBuilder.build(line, x, y, screenW, screenH, Vec4(1, 1, 1, 1), scale);
            vertices.insert(vertices.end(), lineVerts.begin(), lineVerts.end());
            y += lineSpacing;
        }
    }

    renderer.upload(vertices);
    renderer.render(GL_LINES);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        std::cout << "GL error: " << std::hex << err << std::dec << "\n";
}

void drawSquare(void *data)
{
    GLuint vao, vbo, shader;
    float vertices[] = {
        // positions        // colors
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 1.0f, 1.0f, 1.0f};
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    const char *vs = R"(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            layout (location = 1) in vec3 aColor;
            out vec3 vColor;
            void main() {
                vColor = aColor;
                gl_Position = vec4(aPos, 0.0, 1.0);
            }
        )";

    const char *fs = R"(
            #version 330 core
            in vec3 vColor;
            out vec4 FragColor;
            void main() {
                FragColor = vec4(vColor, 1.0);
            }
        )";

    shader = createProgram(vs, fs);

    GLuint ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "[GLFW] Initialization failed!\n";
        return -1;
    }
    std::cout << "[GLFW] Initialized.\n";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create both windows in main thread
    Window radWin("Radar Window");
    radWin.init();
    Window infoWin("Info Window", 600, 600, 0.1f, 0.1f, 0.1f, 1.0f);
    infoWin.init(radWin.getHandle());

    RadarState radarState;
    double lastTime = glfwGetTime();

    radWin.setRenderCallback(drawRadar, &radarState);
    infoWin.setRenderCallback(drawText);

    listener.start();

    while (!glfwWindowShouldClose(radWin.getHandle()) ||
           !glfwWindowShouldClose(infoWin.getHandle()))
    {
        double now = glfwGetTime();
        double dt = now - lastTime;
        lastTime = now;

        // animating 60 degree per second
        radarState.sweepAngle -= 60.0f * dt;
        if (radarState.sweepAngle < 0.0f)
            radarState.sweepAngle += 360.0f;

        radWin.renderOnce();
        infoWin.renderOnce();
        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    listener.stop();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
