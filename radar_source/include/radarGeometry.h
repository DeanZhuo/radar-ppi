#ifndef RadarGeometry_H
#define RadarGeometry_H

#include <vector>
#include "radarGeoType.h"

class RadarGeometry
{
public:
    RadarGeometry(float sweepSpeed = 60.0f, float sweepAngle = 0.0f, float tolerance = 5.0f)
    {
        this->sweepSpeed = sweepSpeed;
        this->sweepAngle = sweepAngle;
        this->detTolerance = tolerance;
        this->scale = 1.0f;
        this->gridColor = Vec4(0.0f, 0.4f, 0.0f, 1.0f);
        this->sweepColor = Vec4(0.0f, 1.0f, 0.0f, 0.4f);
    }

    void setSize(float width)
    {
        this->scale = width;
    }

    void setColors(Vec4 gridColor, Vec4 sweepColor)
    {
        this->gridColor = gridColor;
        this->sweepColor = sweepColor;
    }

    void
    update(float speed, float angle, float tolerance)
    {
        sweepSpeed = speed;
        sweepAngle += angle;
        detTolerance = tolerance;
    }

    float getSweepAngle() const { return sweepAngle; }
    float getTolerance() const { return detTolerance; }

    std::vector<RadarVertex> generateGrid(int rings, int radials, int segment = 100);
    std::vector<RadarVertex> generateSweep(float deltaTime, int segments = 100);

private:
    float sweepSpeed;
    float sweepAngle;
    float detTolerance;
    float scale;
    Vec4 gridColor;
    Vec4 sweepColor;
};

#endif