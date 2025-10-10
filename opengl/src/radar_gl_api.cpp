#include "radar_gl_api.h"
#include <cstring>
#include <fstream>
#include <mutex>
#include <string>
#include <filesystem>
#include <iostream>

#if defined(_WIN32)
#include <windows.h> // must come first
#include <GL/glew.h>
#include <GL/wglew.h>
#else
#include <GL/glew.h>
#include <GL/glxew.h> // for Linux GLX context detection
#endif

std::mutex log_mutex;
void radar_log(const std::string &msg)
{
    std::lock_guard<std::mutex> lock(log_mutex);

    std::ofstream logfile("C:\\Users\\Bee\\Documents\\radar_debug.log", std::ios::app);

    if (logfile.is_open())
    {
        logfile << "[log] " << msg << std::endl;
    }
}

int radar_gl_init()
{
    radar_log("radar_gl_init");

#if defined(_WIN32)
    if (!wglGetCurrentContext())
    {
        radar_log("No GL context current (wglGetCurrentContext returned null)");
        return 0;
    }
#elif defined(__linux__)
    if (!glXGetCurrentContext())
    {
        radar_log("No GL context current (glXGetCurrentContext returned null)");
        return 0;
    }
#endif

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        radar_log("glewInit failed: " + std::string((const char *)glewGetErrorString(err)));
        return 0;
    }
    radar_log("GLEW initialized, version: " + std::string((const char *)glewGetString(GLEW_VERSION)));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

    return 1;
}

void radar_gl_deinit()
{
    radar_log("radar_gl_deinit");
}

RadarContext *radar_create(int rings, int radials, int segment, float sweepSpeed, float tolerance)
{
    radar_log("radar_create");

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        radar_log("GL error before X: " + std::to_string(err));
    }

    auto ctx = new RadarContext;
    ctx->geo = new RadarGeometry(sweepSpeed, 0, tolerance);
    ctx->ringRenderer = new RadarRenderer();
    ctx->radialRenderer = new RadarRenderer();
    ctx->sweepRenderer = new RadarRenderer();

    ctx->ringRenderer->upload(ctx->geo->generateRings(rings, segment));
    ctx->radialRenderer->upload(ctx->geo->generateRadials(radials, segment));

    return ctx;
}

void radar_update_parameter(RadarContext *ctx, float sweepSpeed, float tolerance)
{
    if (!ctx)
        return;

    radar_log("radar_update_parameter");
    ctx->geo->update(sweepSpeed, 0, tolerance);
}

void radar_update_geo(RadarContext *ctx, int rings, int radials, int segment)
{
    if (!ctx)
        return;

    radar_log("radar_update_geo");

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        radar_log("GL error before X: " + std::to_string(err));
    }

    ctx->ringRenderer->upload(ctx->geo->generateRings(rings, segment));
    ctx->radialRenderer->upload(ctx->geo->generateRadials(radials, segment));
}

float radar_render(RadarContext *ctx, int width, int height, double deltaTime)
{
    if (!ctx)
        return 0.0f;

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        radar_log("GL error before X: " + std::to_string(err));
    }

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float baseSize = 600.0f;
    float scale = (float)width / baseSize;

    ctx->ringRenderer->render(GL_LINES);
    ctx->radialRenderer->render(GL_LINES);
    ctx->sweepRenderer->upload(ctx->geo->generateSweep(deltaTime));
    ctx->sweepRenderer->render(GL_TRIANGLE_FAN);

    return ctx->geo->getSweepAngle();
}

void radar_destroy(RadarContext *ctx)
{
    if (!ctx)
        return;

    radar_log("radar_destroy");
    delete ctx->geo;
    delete ctx->ringRenderer;
    delete ctx->radialRenderer;
    delete ctx->sweepRenderer;
    delete ctx;
}
