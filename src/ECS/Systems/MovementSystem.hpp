#ifndef MOVEMENT_SYSTEM_HPP_
#define MOVEMENT_SYSTEM_HPP_

#include "System.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/VelocityComponent.hpp"
#include "../Core/Coordinator.hpp"

class MovementSystem : public System {
public:
    void Update(float dt, Coordinator& coordinator) override {
        for (auto entity : entities) {
            auto& transform = coordinator.GetComponent<Transform>(entity);
            auto& velocity = coordinator.GetComponent<Velocity>(entity);

            transform.position += velocity.linearVelocity * dt;
            // rotation = angularVelocity to be done later
        }
    }
};

#endif
