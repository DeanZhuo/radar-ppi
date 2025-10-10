#include "radar_c_api.h"
#include <cstring>
#include <fstream>
#include <mutex>
#include <string>
#include <filesystem>
#include <iostream>

std::mutex log_mutex; // prevent simultaneous write collisions

// debug
void radar_log(const std::string &msg)
{
    std::lock_guard<std::mutex> lock(log_mutex);

    std::ofstream logfile("C:\\Users\\Bee\\Documents\\radar_debug.log", std::ios::app);

    if (logfile.is_open())
    {
        logfile << "[log] " << msg << std::endl;
    }
}

// Create & destroy
RadarGeometry *radar_geo_create(float sweepSpeed, float sweepAngle, float tolerance)
{
    return new RadarGeometry(sweepSpeed, sweepAngle, tolerance);
}

void radar_geo_destroy(RadarGeometry *geo)
{
    delete geo;
}

// Update
void radar_geo_update(RadarGeometry *geo, float speed, float angle, float tolerance)
{
    geo->update(speed, angle, tolerance);
}

// Generate geometry
int radar_geo_generate_rings(RadarGeometry *geo, int rings, int segment, void *outVerts, int maxVerts)
{
    auto verts = geo->generateRings(rings, segment);
    int count = (int)verts.size();
    char buf[256];

    // sprintf(buf, "radar_geo_generate_rings rings:%d size:%d", rings, count);
    // radar_log(buf);

    int writeCount = std::min(count, maxVerts);
    if (outVerts && writeCount > 0)
    {
        memcpy(outVerts, verts.data(), writeCount * sizeof(RadarVertex));
    }

    return writeCount;
}

int radar_geo_generate_radials(RadarGeometry *geo, int radials, int segment, void *outVerts, int maxVerts)
{
    auto verts = geo->generateRadials(radials, segment);
    int count = (int)verts.size();
    char buf[256];

    // sprintf(buf, "radar_geo_generate_radials radials:%d size:%d", radials, count);
    // radar_log(buf);

    int writeCount = std::min(count, maxVerts);
    if (outVerts && writeCount > 0)
    {
        memcpy(outVerts, verts.data(), writeCount * sizeof(RadarVertex));
    }
    return writeCount;
}

int radar_geo_generate_sweep(RadarGeometry *geo, float deltaTime, int segment, RadarVertex *outVerts, int maxVerts)
{
    auto verts = geo->generateSweep(deltaTime, segment);
    int count = (int)verts.size();
    char buf[256];

    // sprintf(buf, "radar_geo_generate_sweep deltaTime:%.2f size:%d", deltaTime, count);
    // radar_log(buf);

    if (outVerts && maxVerts >= count)
    {
        memcpy(outVerts, verts.data(), count * sizeof(RadarVertex));
    }

    /*
    for (int i = 0; i < count && i < maxVerts; i++)
    {
        sprintf(buf, "SWEEP Vertex %d: X=%.2f Y=%.2f A=%.2f R=%.2f G=%.2f B=%.2f",
                i,
                verts[i].position.x,
                verts[i].position.y,
                verts[i].color.a,
                verts[i].color.r,
                verts[i].color.g,
                verts[i].color.b);
        radar_log(buf);
    }
    */
    return count;
}

float radar_geo_get_angle(RadarGeometry *geo)
{
    return geo->getSweepAngle();
}

float radar_geo_get_tolerance(RadarGeometry *geo)
{
    return geo->getTolerance();
}

int radar_geo_ring_count(RadarGeometry *geo, int rings, int segment)
{
    if (!geo)
        return 0;

    return rings * segment * 2;
}

int radar_geo_radial_count(RadarGeometry *geo, int radials, int segment)
{
    if (!geo)
        return 0;

    return radials * 2;
}

int radar_geo_sweep_count(RadarGeometry *geo, int segments)
{
    if (!geo)
        return 0;

    return 1 + (segments + 1);
}