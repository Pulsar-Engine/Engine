
#include "Window.hpp"

Window::Window(const int width, const int height, const char *title) : _width(width), _height(height), _title(title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _primitive = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void Window::loop() {
    while (!glfwWindowShouldClose(_primitive))
        glfwPollEvents();
}

Window::~Window() {
    glfwDestroyWindow(_primitive);
    glfwTerminate();
}

Window &Window::operator=(const Window &rvalue)
{
    _primitive = rvalue._primitive;
    _width = rvalue._width;
    _height = rvalue._height;
    _title = rvalue._title;
    return *this;
}
