// Radar PPI Display with OpenGL 3 Core Profile using GLFW and GLEW

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>

// Shaders
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
out vec4 vColor;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() {
    FragColor = vColor;
}
)";

// Utility: Compile shader
GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint status;
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << std::endl;
    }
    return s;
}

// Utility: Link program
GLuint createProgram() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// Radar model
constexpr float PI = 3.14159265359f;

struct Target {
    float angle;
    float radius;
    bool detected;
};

// Static targets (polar: angle, radius)
std::vector<Target> targets = {
    {30.0f, 0.6f, false},
    {90.0f, 0.75f, false},
    {135.0f, 0.5f, false},
    {210.0f, 0.85f, false},
    {300.0f, 0.65f, false}
};

float sweep_angle = 0.0f; // degrees
float sweep_speed = 60.0f; // deg/sec
const float det_tolerance = 5.0f;

// Generate grid vertices (rings and radial lines)
void createRadarGrid(std::vector<float>& vertices, std::vector<float>& colors) {
    // Rings
    int rings = 5, seg = 100;
    for (int r = 1; r <= rings; r++) {
        float rad = r * 0.2f;
        for (int i = 0; i <= seg; i++) {
            float th = 2 * PI * i / seg;
            vertices.push_back(rad * cos(th));
            vertices.push_back(rad * sin(th));
            colors.push_back(0.0f);
            colors.push_back(0.4f);
            colors.push_back(0.0f);
            colors.push_back(1.0f);
        }
    }
    // Radial lines (every 30 deg)
    for (int i = 0; i < 12; i++) {
        float th = PI * i * 30.0f / 180.0f;
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        colors.push_back(0.0f);
        colors.push_back(0.4f);
        colors.push_back(0.0f);
        colors.push_back(1.0f);
        vertices.push_back(cos(th));
        vertices.push_back(sin(th));
        colors.push_back(0.0f);
        colors.push_back(0.4f);
        colors.push_back(0.0f);
        colors.push_back(1.0f);
    }
}

// Generate sweep cone vertices
void createSweep(std::vector<float>& vertices, std::vector<float>& colors, float sweep_angle) {
    int seg = 100;
    float width = 5.0f;
    float th0 = sweep_angle - width / 2.0f;
    float th1 = sweep_angle + width / 2.0f;
    vertices.push_back(0.0f); vertices.push_back(0.0f); // center
    colors.push_back(0.0f); colors.push_back(1.0f); colors.push_back(0.0f); colors.push_back(0.4f);
    for (int i = 0; i <= seg; i++) {
        float th = (th0 + (th1-th0)*i/seg) * PI / 180.0f;
        vertices.push_back(cos(th));
        vertices.push_back(sin(th));
        float alpha = 0.4f * (1.0f - float(i)/seg);
        colors.push_back(0.0f); colors.push_back(1.0f); colors.push_back(0.0f); colors.push_back(alpha);
    }
}

// Update detection for target highlight
void updateDetection() {
    for (auto& t : targets) {
        float diff = fabs(t.angle - sweep_angle);
        if (diff > 180.0f) diff = 360.0f - diff;
        t.detected = (diff <= det_tolerance);
    }
}

// Generate target point vertices
void createTargets(std::vector<float>& vertices, std::vector<float>& colors) {
    for (const auto& t : targets) {
        float th = t.angle * PI / 180.0f;
        vertices.push_back(cos(th) * t.radius);
        vertices.push_back(sin(th) * t.radius);
        if (t.detected) {
            colors.push_back(1.0f); colors.push_back(1.0f); colors.push_back(0.0f); colors.push_back(1.0f);
        } else {
            colors.push_back(0.0f); colors.push_back(1.0f); colors.push_back(0.0f); colors.push_back(1.0f);
        }
    }
}

int main() {
    if (!glfwInit()) return 1;
    // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* w = glfwCreateWindow(800, 800, "Radar PPI - OpenGL 3 Core", nullptr, nullptr);
    if (!w) { glfwTerminate(); return 2; }

    glfwMakeContextCurrent(w);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "GLEW fail\n"; return 3; }

    GLuint program = createProgram();
    glUseProgram(program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Buffers/VAOs
    GLuint vao, vbo[2];
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(2, vbo);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(w)) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update sweep angle
        double now = glfwGetTime();
        double dt = now - lastTime;
        lastTime = now;
        sweep_angle += sweep_speed * dt;
        if (sweep_angle > 360.0f) sweep_angle -= 360.0f;
        updateDetection();

        // Radar grid
        std::vector<float> grid_verts, grid_cols;
        createRadarGrid(grid_verts, grid_cols);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, grid_verts.size() * sizeof(float), grid_verts.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0); // pos
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, grid_cols.size() * sizeof(float), grid_cols.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1); // color
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

        // Rings
        int seg = 100;
        for (int r = 0; r < 5; r++) {
            glDrawArrays(GL_LINE_STRIP, r*(seg+1), seg+1);
        }
        // Radial lines
        int gridBase = 5*(seg+1);
        glDrawArrays(GL_LINES, gridBase, 24);

        // Radar sweep
        std::vector<float> sweep_verts, sweep_cols;
        createSweep(sweep_verts, sweep_cols, sweep_angle);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sweep_verts.size() * sizeof(float), sweep_verts.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, sweep_cols.size() * sizeof(float), sweep_cols.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sweep_verts.size()/2);

        // Targets
        std::vector<float> tgt_verts, tgt_cols;
        createTargets(tgt_verts, tgt_cols);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, tgt_verts.size() * sizeof(float), tgt_verts.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, tgt_cols.size() * sizeof(float), tgt_cols.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, tgt_verts.size()/2);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
