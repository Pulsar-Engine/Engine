#ifndef WINDOW_HPP_
    #define WINDOW_HPP_
    
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>

    #include "backend/vulkan/Primitive.hpp"

class Window : public Primitive<GLFWwindow *> {
    public:
        Window() = default;
        Window(const int width, const int height, const char *title);
        ~Window();
        Window &operator=(const Window &rvalue);
        void loop();
    protected:
    private:
        int _width;
        int _height;
        const char *_title;
};

#endif
