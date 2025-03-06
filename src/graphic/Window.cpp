
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
    if (_primitive == nullptr)
        return;
    glfwDestroyWindow(_primitive);
    glfwTerminate();
}
