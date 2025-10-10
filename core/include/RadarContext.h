#ifndef RadarContext_h
#define RadarContext_h

#include "RadarGeometry.h"
#include "RadarRenderer.h"

struct RadarContext
{
    RadarGeometry *geo;
    RadarRenderer *ringRenderer = nullptr;
    RadarRenderer *radialRenderer = nullptr;
    RadarRenderer *sweepRenderer = nullptr;
    double lastTime;
};

#endif