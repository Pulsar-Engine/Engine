#ifndef WINDOW_HPP_
    #define WINDOW_HPP_
    
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>

    #define GLM_FORCE_RADIANS
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>

    #include <chrono>

    #include "backend/vulkan/Primitive.hpp"
    #include "backend/vulkan/Buffer.hpp"

class Instance;

class Window : public Primitive<GLFWwindow *> {
    public:
        Window() = default;
        Window(const int width, const int height, const char *title);
        ~Window();
        Window &operator=(const Window &rvalue);
        void loop(Instance &instance);
        void drawFrame(Instance &instance);
        void callbackResize(GLFWwindow *window, int width, int height);
        void updateUniformBuffer(Buffer &uniformBuffer);
    protected:
    private:
        int _width;
        int _height;
        const char *_title;
        uint32_t _currentFrame;
        bool _framebufferResized;
};

#endif
