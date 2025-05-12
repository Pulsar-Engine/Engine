#include "Engine.hpp"

Instance *Engine::_instance = nullptr;

void Engine::initWindow(bool fromEditor = false)
{
    _instance = new Instance("Pulsar", fromEditor);
}

void Engine::render()
{
    _instance->getWindow()->loop(*_instance);
}

void Engine::destroyWindow()
{
    Engine::close();
    delete _instance;
}

void Engine::togglePause()
{
    _instance->getWindow()->togglePause();
}

void Engine::toggleShow()
{
    _instance->getWindow()->toggleShow();
}

void *Engine::getWindowPtr()
{
    if (_instance == nullptr || _instance->getWindow()->getPrimitive() == nullptr) {
        fprintf(stderr, "Instance or window is null\n");
        return nullptr;
    }
    #ifdef _WIN32
        return (void *) glfwGetWin32Window(_instance->getWindow()->getPrimitive());
    #elif __linux__
        void *libGLFW = dlopen("libglfw.so.3", RTLD_LAZY | RTLD_NOLOAD);
        if (!libGLFW) {
            fprintf(stderr, "Failed to load libglfw.so\n");
            return nullptr;
        }
        glfwGetWindowWayland_t glfwGetWaylandWindowC = (glfwGetWindowWayland_t) dlsym(libGLFW, "glfwGetWaylandWindow");
        if (glfwGetWaylandWindowC)
            return glfwGetWaylandWindowC(_instance->getWindow()->getPrimitive());
        glfwGetWindowX11_t glfwGetX11WindowC = (glfwGetWindowX11_t) dlsym(libGLFW, "glfwGetX11Window");
        if (glfwGetX11WindowC)
            return (void *) glfwGetX11WindowC(_instance->getWindow()->getPrimitive());
        fprintf(stderr, "Failed to load glfwGetWaylandWindow or glfwGetX11Window\n");
        return nullptr;
    #else
        fprintf(stderr, "Unsupported platform\n");
        return nullptr;
    #endif
}

void Engine::close()
{
    _instance->getWindow()->close();
}