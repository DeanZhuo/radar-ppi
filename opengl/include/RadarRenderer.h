#ifndef RadarRenderer_H
#define RadarRenderer_H

#include "RadarGeometry.h"

class RadarRenderer
{
public:
    RadarRenderer() : VAO(0), VBO(0), vertexCount(0)
    {
        CreateShaderProgram();
    }
    ~RadarRenderer();

    void upload(const std::vector<RadarVertex> &vertices);
    void render(unsigned int drawMode);

private:
    const char *vertexShaderSrc = R"(#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
out vec4 vColor;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

    const char *fragmentShaderSrc = R"(#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() {
    FragColor = vColor;
}
)";

    unsigned int VAO, VBO, vertexCount;
    unsigned int shaderProgram;

    void cleanup();
    void CreateShaderProgram();
    unsigned int compileShader(unsigned int type, const char *src);
    void checkShaderProgramErrors(unsigned int shaderProgram);
};

#endif