#ifndef RADAR_H
#define RADAR_H

#include <vector>
#include "helper.h"

class Radar
{
public:
    Radar(float sweepSpeed = 60.0f, float sweepAngle = 0.0f, float detectionTolerance = 5.0f);

    ~Radar();

    void update(float speed, float angle, float tolerance);

    void initTarget();
    void updateTargets(std::vector<Target> targets);
    void drawTargets();

    void createRadarGrid(int numRings, int numRadials);
    void drawGrid();

    void initSweep();
    void updateSweep(float deltaTime);

private:
    struct RadarObject
    {
        unsigned int VAO;
        unsigned int VBOPos;
        unsigned int VBOCol;
        unsigned int vertexCount;

        void cleanup()
        {
            if (VBOPos)
            {
                glDeleteBuffers(1, &VBOPos);
                VBOPos = 0;
            }
            if (VBOCol)
            {
                glDeleteBuffers(1, &VBOCol);
                VBOCol = 0;
            }
            if (VAO)
            {
                glDeleteVertexArrays(1, &VAO);
                VAO = 0;
            }
            vertexCount = 0;
        }
    };

    RadarObject grid{};
    RadarObject sweep{};
    RadarObject targetsObj{};

    float sweep_speed;   // degrees per second
    float det_tolerance; // degrees
    float sweep_angle;   // current angle
    std::vector<Target> targets;

    void createSweep(std::vector<float> &vertices, std::vector<float> &colors);
    void createTargets(std::vector<float> &vertices, std::vector<float> &colors);
};
#endif