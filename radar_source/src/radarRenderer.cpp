#include "radarRenderer.h"
#include <GL/glew.h>

RadarRenderer::~RadarRenderer()
{
    cleanup();
}

void RadarRenderer::upload(const std::vector<RadarVertex> &vertices)
{
    cleanup();
    vertexCount = vertices.size();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(RadarVertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RadarVertex),
                          (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RadarVertex),
                          (void *)(sizeof(Vec2)));

    // cleanup state
    glBindVertexArray(0);
}

void RadarRenderer::render(unsigned int drawMode)
{
    if (vertexCount == 0)
        return;

    glBindVertexArray(VAO);
    glDrawArrays(drawMode, 0, vertexCount);
    glBindVertexArray(0);
}

void RadarRenderer::cleanup()
{
    if (VBO)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    vertexCount = 0;
}
