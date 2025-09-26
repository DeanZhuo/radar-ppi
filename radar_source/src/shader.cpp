#include <GL/glew.h>
#include "shader.h"

Shader::Shader(const char *fragmentShaderSrc, const char *vertexShaderSrc)
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
        this->Program = prog;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

Shader::~Shader()
{
    glDeleteProgram(this->Program);
}

void Shader::use()
{
    glUseProgram(this->Program);
}

unsigned int Shader::compileShader(unsigned int type, const char *src)
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
        std::cerr << "Shader compile error: " << log << std::endl;
    }
    return s;
}

void Shader::checkShaderProgramErrors(unsigned int shaderProgram)
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