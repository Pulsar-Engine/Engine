#include "SceneManager.hpp"
#include "../Core/Coordinator.hpp"

SceneManager::SceneManager(Coordinator& coordinator)
    : _coordinator(coordinator) {}

Scene& SceneManager::createScene(const std::string& name) {
    _scenes[name] = Scene(name);
    _currentScene = &_scenes[name];
    return *_currentScene;
}

void SceneManager::switchToScene(const std::string& name) {
    auto it = _scenes.find(name);
    if (it != _scenes.end()) {
        _currentScene = &it->second;
    }
}

void SceneManager::removeScene(const std::string& name) {
    if (_scenes.find(name) != _scenes.end()) {
        if (_currentScene && _currentScene->name == name)
            _currentScene = nullptr;
        _scenes.erase(name);
    }
}

Scene* SceneManager::getCurrentScene() {
    return _currentScene;
}

Scene* SceneManager::getScene(const std::string& name) {
    auto it = _scenes.find(name);
    if (it != _scenes.end())
        return &it->second;
    return nullptr;
}

Entity SceneManager::createEntityInCurrentScene() {
    if (!_currentScene)
        throw std::runtime_error("No scene currently loaded.");
    Entity entity = _coordinator.CreateEntity();
    _currentScene->entities.push_back(entity);
    return entity;
}
