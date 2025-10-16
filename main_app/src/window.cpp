#include "Window.h"

Window::Window(const std::string &title, int width, int height, float r, float g, float b, float a)
    : title(title), width(width), height(height), r(r), g(g), b(b), a(a) {}

Window::~Window()
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
}

bool Window::init(GLFWwindow *share)
{
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, share);
    if (!window)
    {
        const char *desc;
        int code = glfwGetError(&desc);
        std::cerr << "[GLFW] Failed to create window: " << title
                  << "\nError (" << code << "): " << (desc ? desc : "Unknown") << "\n";
        return false;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "[GLEW] Initialization failed in window (" << title << "): "
                  << glewGetErrorString(err) << "\n";
        return false;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    std::cout << "[GLFW] Framebuffer size for " << title << ": " << fbWidth << "x" << fbHeight << "\n";

    return true;
}

void Window::renderOnce()
{
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);

    if (renderCallback)
        renderCallback(userData);

    glfwSwapBuffers(window);
}