#ifndef WINDOW_HPP_
    #define WINDOW_HPP_
    
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>

class Window {
    public:
        Window(const int width, const int height, const char *title);
        ~Window();
        void loop();
    protected:
    private:
        int width;
        int height;
        const char *title;
        GLFWwindow *primitiveWindow;
};

#endif
