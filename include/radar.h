#ifndef RADAR_H
#define RADAR_H

#include <vector>

struct RadarObject
{
    unsigned int VAO;
    unsigned int VBOPos;
    unsigned int VBOCol;
    unsigned int vertexCount;

    void cleanup();
};

class Radar
{
public:
    const float PI = 3.14159265358979323846f;

    Radar(float sweepSpeed = 60.0f, float sweepAngle = 0.0f, float detectionTolerance = 5.0f);

    ~Radar();

    void update(float speed, float angle, float tolerance);
    float getSweepAngle() const { return sweep_angle; }
    float getTolerance() const { return det_tolerance; }

    void createRadarGrid(int numRings, int numRadials);
    void drawGrid();

    void initSweep();
    void updateSweep(float deltaTime);

private:
    RadarObject grid{};
    RadarObject sweep{};

    float sweep_speed;   // degrees per second
    float det_tolerance; // degrees
    float sweep_angle;   // current angle

    void createSweep(std::vector<float> &vertices, std::vector<float> &colors);
};
#endif