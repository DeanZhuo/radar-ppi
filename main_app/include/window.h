#ifndef Window_H
#define Window_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <thread>
#include <mutex>

class Window
{
public:
    Window(const std::string &title, int width = 600, int height = 600, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    ~Window();

    void setRenderCallback(std::function<void(void *)> callback, void *data = nullptr)
    {
        this->renderCallback = callback;
        this->userData = data;
    }

    GLFWwindow *getHandle() const { return window; }
    void renderOnce();
    bool init(GLFWwindow *share = nullptr);

private:
    std::string title;
    int width;
    int height;
    float r, g, b, a;

    GLFWwindow *window = nullptr;
    std::function<void(void *)> renderCallback;
    void *userData = nullptr;
};

#endif