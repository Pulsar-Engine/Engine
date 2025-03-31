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
    if (_instance == nullptr || _instance->getWindow()->getPrimitive() == nullptr)
        return nullptr;
    #ifdef _WIN32
        return (void *) glfwGetWin32Window(_instance->getWindow()->getPrimitive());
    #elif __linux__
        return (void *) glfwGetX11Window(_instance->getWindow()->getPrimitive());
    #else
        return nullptr;
    #endif
}

void Engine::close()
{
    _instance->getWindow()->close();
}