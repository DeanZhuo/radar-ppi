#include "RadarGeometry.h"
#include <cmath>

std::vector<RadarVertex> RadarGeometry::generateRings(int rings, int segment)
{
    std::vector<RadarVertex> result;

    for (int r = 1; r <= rings; r++)
    {
        float rad = r * 0.2f;
        for (int i = 0; i <= segment; i++)
        {
            float th = 2 * PI * i / segment;
            RadarVertex seg;
            seg.position = Vec2(rad * cos(th) * scale, rad * sin(th) * scale);
            seg.color = gridColor;
            result.push_back(seg);
        }
    }

    return result;
}

std::vector<RadarVertex> RadarGeometry::generateRadials(int radials, int segment)
{
    std::vector<RadarVertex> result;

    for (int i = 0; i < radials; i++)
    {
        float th = (2 * PI * i) / radials;
        RadarVertex pointA;
        pointA.position = Vec2();
        pointA.color = gridColor;
        result.push_back(pointA);

        RadarVertex pointB;
        pointB.position = Vec2(cos(th) * scale, sin(th) * scale);
        pointB.color = gridColor;
        result.push_back(pointB);
    }

    return result;
}

std::vector<RadarVertex> RadarGeometry::generateSweep(float deltaTime, int segments)
{
    std::vector<RadarVertex> result;

    sweepAngle -= sweepSpeed * deltaTime;
    if (sweepAngle < 0.0f)
        sweepAngle += 360.0f;

    float th0 = sweepAngle - detTolerance / 2.0f;
    float th1 = sweepAngle + detTolerance / 2.0f;

    RadarVertex center;
    center.position = Vec2();
    center.color = sweepColor;
    result.push_back(center);

    for (int i = 0; i <= segments; i++)
    {
        float th = (th0 + (th1 - th0) * i / segments) * PI / 180.0f;
        float alpha = sweepColor.a * (1.0f - float(i) / segments);

        RadarVertex seg;
        seg.position = Vec2(cos(th) * scale, sin(th) * scale);
        seg.color = Vec4(sweepColor.r, sweepColor.g, sweepColor.b, alpha);
        result.push_back(seg);
    }

    return result;
}