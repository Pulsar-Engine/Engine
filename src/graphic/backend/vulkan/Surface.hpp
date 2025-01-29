
#ifndef SURFACE_HPP_
    #define SURFACE_HPP_

    #include "Primitive.hpp"

    #ifdef _WIN32
        #define GLFW_EXPOSE_NATIVE_WIN32
        #define VK_USE_PLATFORM_WIN32_KHR
    #endif

    #define GLFW_INCLUDE_VULKAN

    #include <GLFW/glfw3.h>


    #include <GLFW/glfw3native.h>

class Surface : public Primitive<VkSurfaceKHR> {
    public:
        Surface() = default;
        Surface(VkInstance *instance, GLFWwindow *window);
        ~Surface();
        Surface &operator=(const Surface &rvalue);
    protected:
    private:
        VkInstance *_instance;
        GLFWwindow *_window;
    
};

#endif
