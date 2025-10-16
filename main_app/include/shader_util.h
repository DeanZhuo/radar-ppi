#ifndef shader_util_h
#define shader_util_h

#include <GL/glew.h>
#include <string>
#include <iostream>

GLuint compileShader(GLenum type, const std::string &source);
GLuint createProgram(const std::string &vsSource, const std::string &fsSource);

#endif