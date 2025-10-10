#ifndef RADAR_GL_API_H
#define RADAR_GL_API_H

#include "RadarContext.h"

#ifdef _WIN32
#ifdef RADAR_BUILD_DLL
#define RADAR_API extern "C" __declspec(dllexport)
#else
#define RADAR_API extern "C" __declspec(dllimport)
#endif
#define RADAR_CALL __cdecl
#else
#define RADAR_API extern "C"
#define RADAR_CALL
#endif

RADAR_API int radar_gl_init();
RADAR_API RadarContext *radar_create(int rings, int radials, int segment, float sweepSpeed, float tolerance);
RADAR_API void radar_update_parameter(RadarContext *ctx, float sweepSpeed, float tolerance);
RADAR_API void radar_update_geo(RadarContext *ctx, int rings, int radials, int segment);
RADAR_API float radar_render(RadarContext *ctx, int width, int height, double deltaTime);
RADAR_API void radar_destroy(RadarContext *ctx);
RADAR_API void radar_gl_deinit();

#endif
