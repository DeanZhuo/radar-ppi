#include <GL/glew.h>
#include "radar.h"
#include <cmath>
#include <iostream>

Radar::Radar(float sweepSpeed, float sweepAngle, float detectionTolerance)
{
    sweep_speed = sweepSpeed;
    sweep_angle = sweepAngle;
    det_tolerance = detectionTolerance;
}

Radar::~Radar()
{
    grid.cleanup();
    sweep.cleanup();
}

void Radar::update(float speed, float angle, float tolerance)
{
    sweep_speed = speed;
    sweep_angle += angle;
    det_tolerance = tolerance;
}

void Radar::drawGrid()
{
    glBindVertexArray(grid.VAO);

    int seg = 100;
    // draw rings
    for (int r = 0; r < 5; r++)
    {
        glDrawArrays(GL_LINE_STRIP, r * (seg + 1), seg + 1);
    }

    // draw radials
    int gridBase = 5 * (seg + 1);
    glDrawArrays(GL_LINES, gridBase, 2 * 12);

    glBindVertexArray(0);
}

void Radar::initSweep()
{
    sweep.cleanup();
    glGenVertexArrays(1, &sweep.VAO);
    glGenBuffers(1, &sweep.VBOPos);
    glGenBuffers(1, &sweep.VBOCol);

    glBindVertexArray(sweep.VAO);
    int seg = 102; // 100 + center + repeat last

    // Position buffer
    glBindBuffer(GL_ARRAY_BUFFER, sweep.VBOPos);
    glBufferData(GL_ARRAY_BUFFER, seg * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Color buffer
    glBindBuffer(GL_ARRAY_BUFFER, sweep.VBOCol);
    glBufferData(GL_ARRAY_BUFFER, seg * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    sweep.vertexCount = seg;
    glBindVertexArray(0);
}

void Radar::updateSweep(float deltaTime)
{
    sweep_angle -= sweep_speed * deltaTime;
    if (sweep_angle < 0.0f)
        sweep_angle += 360.0f;

    std::vector<float> verts, cols;

    createSweep(verts, cols);

    glBindVertexArray(sweep.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, sweep.VBOPos);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(float), verts.data());

    glBindBuffer(GL_ARRAY_BUFFER, sweep.VBOCol);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cols.size() * sizeof(float), cols.data());

    glDrawArrays(GL_TRIANGLE_FAN, 0, verts.size() / 2);
    glBindVertexArray(0);
}

void Radar::createRadarGrid(int numRings, int numRadials)
{
    grid.cleanup();

    std::vector<float> vertices;
    std::vector<float> colors;

    // Rings
    int seg = 100;
    for (int r = 1; r <= numRings; r++)
    {
        float rad = r * 0.2f;
        for (int i = 0; i <= seg; i++)
        {
            float th = 2 * PI * i / seg;
            vertices.push_back(rad * cos(th));
            vertices.push_back(rad * sin(th));
            colors.push_back(0.0f);
            colors.push_back(0.4f);
            colors.push_back(0.0f);
            colors.push_back(1.0f);
        }
    }

    // Radial lines
    for (int i = 0; i < numRadials; i++)
    {
        float th = PI * i * 30.0f / 180.0f;
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        colors.push_back(0.0f);
        colors.push_back(0.4f);
        colors.push_back(0.0f);
        colors.push_back(1.0f);
        vertices.push_back(cos(th));
        vertices.push_back(sin(th));
        colors.push_back(0.0f);
        colors.push_back(0.4f);
        colors.push_back(0.0f);
        colors.push_back(1.0f);
    }

    grid.vertexCount = vertices.size() / 2;
    glGenVertexArrays(1, &grid.VAO);
    glBindVertexArray(grid.VAO);

    glGenBuffers(1, &grid.VBOPos);
    glBindBuffer(GL_ARRAY_BUFFER, grid.VBOPos);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // layout(location=0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &grid.VBOCol);
    glBindBuffer(GL_ARRAY_BUFFER, grid.VBOCol);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1); // layout(location=1)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    // cleanup state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Radar::createSweep(std::vector<float> &vertices, std::vector<float> &colors)
{
    int seg = 100;
    float width = 5.0f;
    float th0 = sweep_angle - width / 2.0f;
    float th1 = sweep_angle + width / 2.0f;

    vertices.push_back(0.0f);
    vertices.push_back(0.0f); // center
    colors.push_back(0.0f);
    colors.push_back(1.0f);
    colors.push_back(0.0f);
    colors.push_back(0.4f);

    for (int i = 0; i <= seg; i++)
    {
        float th = (th0 + (th1 - th0) * i / seg) * PI / 180.0f;
        vertices.push_back(cos(th));
        vertices.push_back(sin(th));
        float alpha = 0.4f * (1.0f - float(i) / seg);
        colors.push_back(0.0f);
        colors.push_back(1.0f);
        colors.push_back(0.0f);
        colors.push_back(alpha);
    }
}

void RadarObject::cleanup()
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