#ifndef RadarRenderer_H
#define RadarRenderer_H

#include "radarGeometry.h"

class RadarRenderer
{
public:
    RadarRenderer() : VAO(0), VBO(0), vertexCount(0) {}
    ~RadarRenderer();

    void upload(const std::vector<RadarVertex> &vertices);
    void render(unsigned int drawMode);

private:
    unsigned int VAO, VBO;
    unsigned int vertexCount;

    void cleanup();
};

#endif