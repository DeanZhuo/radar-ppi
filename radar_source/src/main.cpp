// Radar PPI Display with OpenGL 3 Core Profile using GLFW and GLEW

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>

#include "helper.h"
#include "shader.h"
// #include "radar.h"
#include "radarGeometry.h"
#include "radarRenderer.h"
#include "target.h"

const char *vertexShaderSrc = R"(#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
out vec4 vColor;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

const char *fragmentShaderSrc = R"(#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() {
    FragColor = vColor;
}
)";

// Static targets (polar: angle, radius)
std::vector<Target> targets = {
    {30.0f, 0.6f, false, 10.0f, 0.0f},
    {90.0f, 0.75f, false, -15.0f, 0.0f},
    {135.0f, 0.5f, false, 20.0f, 0.0f},
    {180.0f, 0.9f, false, -5.0f, 0.0f},
    {210.0f, 0.85f, false, 0.0f, -0.1f},
    {240.0f, 0.4f, false, 0.0f, 0.2f},
    {300.0f, 0.65f, false, -10.0f, 0.0f},
    {330.0f, 0.3f, false, 5.0f, 0.0f},
};

float sweep_angle = 0.0f;  // degrees
float sweep_speed = 60.0f; // deg/sec
float det_tolerance = 5.0f;

int main()
{
    if (!glfwInit())
        return 1;
    // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *w = glfwCreateWindow(800, 800, "Radar PPI - OpenGL 3 Core", nullptr, nullptr);
    if (!w)
    {
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(w);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW fail\n";
        return 3;
    }

    Shader radarShader(fragmentShaderSrc, vertexShaderSrc);
    radarShader.use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*
    Radar radar(sweep_speed, sweep_angle, det_tolerance);
    radar.createRadarGrid(5, 12);
    radar.initSweep();
    */

    RadarGeometry geo(sweep_speed, sweep_angle, det_tolerance);
    RadarRenderer gridRenderer, sweepRenderer;
    auto gridVerts = geo.generateGrid(5, 12);
    gridRenderer.upload(gridVerts);

    TargetManager targetManager;
    targetManager.targets = targets;
    targetManager.init();

    double lastTime = glfwGetTime();
    glClearColor(0, 0, 0, 1);
    float sweepAngle, detTolerance;

    while (!glfwWindowShouldClose(w))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        double now = glfwGetTime();
        double dt = now - lastTime;
        lastTime = now;

        auto sweepVerts = geo.generateSweep(dt);
        sweepRenderer.upload(sweepVerts);

        gridRenderer.render(GL_LINE_STRIP);
        gridRenderer.render(GL_LINE);
        sweepRenderer.render(GL_TRIANGLE_FAN);

        sweepAngle = geo.getSweepAngle();
        detTolerance = geo.getTolerance();

        /*
        radar.drawGrid();
        radar.updateSweep(dt);
        sweepAngle = radar.getSweepAngle();
        detTolerance = radar.getTolerance();
        */

        targetManager.animate(dt);
        targetManager.detect(sweepAngle, detTolerance);
        targetManager.draw();

        glfwSwapBuffers(w);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
