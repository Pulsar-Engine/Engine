
#include "Window.hpp"

Window::Window(const int width, const int height, const char *title) : width(width), height(height), title(title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    primitiveWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void Window::loop() {
    while (!glfwWindowShouldClose(primitiveWindow))
        glfwPollEvents();
}

Window::~Window() {
    glfwDestroyWindow(primitiveWindow);
    glfwTerminate();
}
