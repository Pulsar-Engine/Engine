#include "Surface.hpp"

Surface::Surface(VkInstance *instance, GLFWwindow *window) : _instance(instance), _window(window)
{
    if (glfwCreateWindowSurface(*_instance, _window, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}

Surface::~Surface()
{
    if (_instance == nullptr)
        return;
    vkDestroySurfaceKHR(*_instance, _primitive, nullptr);
}

Surface &Surface::operator=(const Surface &rvalue)
{
    _primitive = rvalue._primitive;
    _instance = rvalue._instance;
    _window = rvalue._window;
    return *this;
}

