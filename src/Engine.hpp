#ifndef ENGINE_HPP_
    #define ENGINE_HPP_

    #include "graphic/backend/vulkan/Instance.hpp"

    #ifdef _WIN32
        #define ENGINE_API __declspec(dllexport)
    #else
        #define ENGINE_API __attribute__((visibility("default")))
        #include <dlfcn.h>

        typedef void *(*glfwGetWindowWayland_t)(GLFWwindow *);
        typedef unsigned long (*glfwGetWindowX11_t)(GLFWwindow *);
    #endif

class Engine {
    public:
        ENGINE_API static void close();
        ENGINE_API static void initWindow(bool fromEditor);
        ENGINE_API static void render();
        ENGINE_API static void destroyWindow();
        ENGINE_API static void togglePause();
        ENGINE_API static void toggleShow();
        ENGINE_API static void* getWindowPtr();
    protected:
    private:
        static Instance* _instance;
};

#endif /* !ENGINE_HPP_ */
