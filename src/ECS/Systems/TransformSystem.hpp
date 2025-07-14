#ifndef TRANSFORM_SYSTEM_HPP_
#define TRANSFORM_SYSTEM_HPP_

#include "System.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Core/Coordinator.hpp"
#include <GLFW/glfw3.h>

class TransformSystem : public System {
public:
    void Update(float dt, Coordinator& coordinator) override {
        for (auto entity : entities) {
            auto& transform = coordinator.GetComponent<TransformComponent>(entity);
            // Exemple : oscillation sinusoïdale sur l'axe Y
            transform.position.y = sinf(glfwGetTime()) * 2.0f;
        }
    }
};

#endif
