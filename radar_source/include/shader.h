#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    Shader(const char *fragmentShaderSrc, const char *vertexShaderSrc);
    ~Shader();
    void use();

private:
    unsigned int Program;
    unsigned int compileShader(unsigned int type, const char *source);
    void checkShaderProgramErrors(unsigned int shaderProgram);
};

#endif