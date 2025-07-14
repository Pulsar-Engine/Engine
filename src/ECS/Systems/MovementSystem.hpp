#ifndef MOVEMENT_SYSTEM_HPP_
    #define MOVEMENT_SYSTEM_HPP_

    #include "System.hpp"
    #include "../Components/TransformComponent.hpp"
    #include "../Components/VelocityComponent.hpp"
    #include "../Components/MeshComponent.hpp"
    #include "../Components/GravityComponent.hpp"
    #include "../Core/Coordinator.hpp"

class MovementSystem : public System {
public:
    void Update(float dt, Coordinator& coordinator) override {
        for (auto entity : entities) {
            auto& transform = coordinator.GetComponent<TransformComponent>(entity);
            auto& velocity = coordinator.GetComponent<Velocity>(entity);
            auto& mesh = coordinator.GetComponent<MeshComponent>(entity);
            auto& gravity = coordinator.GetComponent<GravityComponent>(entity);
            gravity.speed += gravity.gravity * dt;
            if (gravity.speed > gravity.maxSpeed)
                gravity.speed = gravity.maxSpeed;
            velocity.linearVelocity.y = -static_cast<float>(gravity.speed);
            transform.position += velocity.linearVelocity * dt;
            mesh.mesh->setPosition(transform.position);
        }
    }
};

#endif
