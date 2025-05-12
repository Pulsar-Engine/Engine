#ifndef GWINDOW_HPP_
    #define GWINDOW_HPP_
    
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>

    #define GLM_FORCE_RADIANS
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>

    #include <chrono>
    #include <atomic>

    #include "backend/vulkan/Primitive.hpp"
    #include "backend/vulkan/Buffer.hpp"

class Instance;
class GWindow : public Primitive<GLFWwindow *> {
    public:
        GWindow() = default;
        GWindow(const int width, const int height, const char *title, bool fromEditor);
        ~GWindow();
        GWindow &operator=(const GWindow &rvalue);
        void loop(Instance &instance);
        void drawFrame(Instance &instance);
        void callbackResize(GLFWwindow *window, int width, int height);
        void updateUniformBuffer(Buffer &uniformBuffer);
        static GWindow *getInstance();
        void close();
        void togglePause();
        void toggleShow();
    protected:
    private:
        static GWindow *_instance;
        int _width;
        int _height;
        const char *_title;
        uint32_t _currentFrame;
        bool _framebufferResized;
        std::atomic<bool> _finished;
        std::atomic<bool> _paused;
};

#endif
