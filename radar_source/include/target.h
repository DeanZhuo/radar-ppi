#ifndef TARGET_H
#define TARGET_H

#include <vector>
#include "radar.h"

struct Target
{
    float angle;
    float radius;
    bool detected;
    float angularVelocity;
    float radialVelocity;
};

class TargetManager
{
public:
    std::vector<Target> targets;

    void animate(float dt);
    void detect(float sweepAngle, float detTolerance);
    void init();
    void draw();

private:
    const float PI = 3.14159265358979323846f;

    RadarObject targetsObj{};
};

#endif