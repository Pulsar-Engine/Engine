#ifndef SCENEMANAGER_HPP_
#define SCENEMANAGER_HPP_

#include "../Core/Types.hpp"
#include <unordered_map>
#include <string>
#include <vector>

class Coordinator;

class Scene {
public:
    std::string name;
    std::vector<Entity> entities;

    Scene(const std::string& name = "") : name(name) {}
};

class SceneManager {
public:
    SceneManager(Coordinator& coordinator);

    Scene& createScene(const std::string& name);
    void switchToScene(const std::string& name);
    void removeScene(const std::string& name);

    Scene* getCurrentScene();
    Scene* getScene(const std::string& name);

    Entity createEntityInCurrentScene();

private:
    Coordinator& _coordinator;
    std::unordered_map<std::string, Scene> _scenes;
    Scene* _currentScene = nullptr;
};

#endif
