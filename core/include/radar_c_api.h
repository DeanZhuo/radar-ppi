#ifndef radar_c_api_h
#define radar_c_api_h

#include "RadarGeometry.h"

#ifdef _WIN32
#ifdef RADAR_BUILD_DLL
#define RADAR_API __declspec(dllexport)
#else
#define RADAR_API __declspec(dllimport)
#endif
#else
#define RADAR_API
#endif

extern "C"
{
    // Create & destroy
    RADAR_API RadarGeometry *radar_geo_create(float sweepSpeed, float sweepAngle, float tolerance);
    RADAR_API void radar_geo_destroy(RadarGeometry *geo);

    // Update
    RADAR_API void radar_geo_update(RadarGeometry *geo, float speed, float angle, float tolerance);

    // Generate geometry
    RADAR_API int radar_geo_generate_rings(RadarGeometry *geo, int rings, int segment, void *outVerts, int maxVerts);
    RADAR_API int radar_geo_generate_radials(RadarGeometry *geo, int radials, int segment, void *outVerts, int maxVerts);
    RADAR_API int radar_geo_generate_sweep(RadarGeometry *geo, float deltaTime, int segment, RadarVertex *outVerts, int maxVerts);

    RADAR_API float radar_geo_get_angle(RadarGeometry *geo);
    RADAR_API float radar_geo_get_tolerance(RadarGeometry *geo);

    // --- Count how many vertices the grid will need
    RADAR_API int radar_geo_ring_count(RadarGeometry *geo, int rings, int segment);
    RADAR_API int radar_geo_radial_count(RadarGeometry *geo, int radials, int segment);

    // --- Count how many vertices the sweep will need
    RADAR_API int radar_geo_sweep_count(RadarGeometry *geo, int segments);
}

#endif
