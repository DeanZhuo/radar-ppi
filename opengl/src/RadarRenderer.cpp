#include "RadarRenderer.h"
#include <GL/glew.h>
#include <iostream>

RadarRenderer::RadarRenderer() : vertexCount(0)
{
    CreateShaderProgram();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
}

RadarRenderer::~RadarRenderer()
{
    cleanup();
    glDeleteProgram(this->shaderProgram);
}

void RadarRenderer::upload(const std::vector<RadarVertex> &vertices)
{
    vertexCount = vertices.size();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(RadarVertex), vertices.data(), GL_DYNAMIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RadarVertex),
                          (void *)0);

    // rgba
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

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(drawMode, 0, vertexCount);
    glBindVertexArray(0);
    glUseProgram(0);
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

void RadarRenderer::CreateShaderProgram()
{
    try
    {
        GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);

        checkShaderProgramErrors(prog);

        glDeleteShader(vs);
        glDeleteShader(fs);

        // Store the program ID
        this->shaderProgram = prog;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

unsigned int RadarRenderer::compileShader(unsigned int type, const char *src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint status;
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        throw std::runtime_error(std::string("Shader compile error: ") + log);
    }
    return s;
}

void RadarRenderer::checkShaderProgramErrors(unsigned int shaderProgram)
{
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        throw std::runtime_error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog));
    }
}