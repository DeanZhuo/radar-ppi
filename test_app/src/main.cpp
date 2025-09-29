#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <RadarGeometry.h>
#include <RadarRenderer.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

int main()
{
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // fix compilation on OS X
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, 800, 800);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "GLEW init failed: " << glewGetErrorString(err) << "\n";
        return 3;
    }

    // enable OpenGL debug context (only works if we requested it)
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(
            [](GLenum source, GLenum type, GLuint id,
               GLenum severity, GLsizei length,
               const GLchar *message,
               const void *userParam)
            {
                std::cerr << "GL ERROR: " << message << std::endl;
            },
            nullptr);
    }

    // set clear color
    glClearColor(0, 0, 0, 1);

    RadarGeometry geo(60, 0, 5);
    RadarRenderer ringRenderer, radialRenderer, sweepRenderer;
    auto ringverts = geo.generateRings(5);
    ringRenderer.upload(ringverts);
    auto radialverts = geo.generateRadials(12);
    radialRenderer.upload(radialverts);
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        double now = glfwGetTime();
        double dt = now - lastTime;
        lastTime = now;
        auto sweepVerts = geo.generateSweep(dt);
        sweepRenderer.upload(sweepVerts);

        ringRenderer.render(GL_LINE_STRIP);
        radialRenderer.render(GL_LINES);
        sweepRenderer.render(GL_TRIANGLE_FAN);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
