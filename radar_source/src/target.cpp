#include <GL/glew.h>
#include "target.h"
#include <vector>
#include <cmath>

void TargetManager::animate(float dt)
{
    for (auto &t : targets)
    {
        t.angle += t.angularVelocity * dt;
        if (t.angle >= 360.0f)
            t.angle -= 360.0f;
        if (t.angle < 0.0f)
            t.angle += 360.0f;

        t.radius += t.radialVelocity * dt;
        if (t.radius < 0.1f)
            t.radius = 0.1f;
        if (t.radius > 1.0f)
            t.radius = 1.0f;
    }
}

void TargetManager::detect(float sweepAngle, float detTolerance)
{
    for (auto &t : targets)
    {
        float diff = fmodf(fabsf(t.angle - sweepAngle), 360.0f);
        if (diff > 180.0f)
            diff = 360.0f - diff;

        t.detected = (diff <= detTolerance);
    }
}

void TargetManager::init()
{
    targetsObj.cleanup();
    glGenVertexArrays(1, &targetsObj.VAO);
    glGenBuffers(1, &targetsObj.VBOPos);
    glGenBuffers(1, &targetsObj.VBOCol);
}

void TargetManager::draw()
{
    std::vector<float> vertices;
    std::vector<float> colors;
    vertices.reserve(targets.size() * 2);
    colors.reserve(targets.size() * 4);

    for (auto &t : targets)
    {
        float th = t.angle * PI / 180.0f;
        vertices.push_back(cos(th) * t.radius);
        vertices.push_back(sin(th) * t.radius);
        if (t.detected)
        {
            colors.push_back(1.0f);
            colors.push_back(1.0f);
            colors.push_back(0.0f);
            colors.push_back(1.0f);
        }
        else
        {
            colors.push_back(0.0f);
            colors.push_back(1.0f);
            colors.push_back(0.0f);
            colors.push_back(1.0f);
        }
    }

    targetsObj.vertexCount = targets.size();

    glBindVertexArray(targetsObj.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, targetsObj.VBOPos);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, targetsObj.VBOCol);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    if (targetsObj.vertexCount == 0)
        return;

    glBindVertexArray(targetsObj.VAO);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, targetsObj.vertexCount);
    glBindVertexArray(0);
}