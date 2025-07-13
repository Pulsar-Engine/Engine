#pragma once

#include "System.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/MeshComponent.hpp"
#include "../Components/CameraComponent.hpp"
#include "../Core/Coordinator.hpp"

#include <glm/gtc/matrix_transform.hpp>

class RenderSystem : public System {
public:
    void Update(Coordinator& coordinator, VkCommandBuffer commandBuffer, uint32_t currentFrame, uint32_t cameraEntity) {
        // Récupère la caméra
        auto& camera = coordinator.GetComponent<CameraComponent>(cameraEntity);

        glm::mat4 view = glm::lookAt(
            camera.position,
            camera.target,
            camera.up
        );

        glm::mat4 proj = glm::perspective(
            glm::radians(camera.fov),
            16.0f / 9.0f, 
            camera.nearClip,
            camera.farClip
        );
        proj[1][1] *= -1; // Correction pour Vulkan (inversion Y)

        // Pour chaque entité avec un mesh et un transform
        for (auto entity : entities) {
            auto& meshComp = coordinator.GetComponent<MeshComponent>(entity);
            auto& transform = coordinator.GetComponent<TransformComponent>(entity);

            // Met à jour la transformation du mesh
            meshComp.mesh->setPosition(transform.position);
            meshComp.mesh->setRotation(transform.rotation);
            meshComp.mesh->setScale(transform.scale);

            // Met à jour les UBO
            meshComp.mesh->updateUniformBuffer(currentFrame, view, proj);

            // Render
            meshComp.mesh->bindAndDraw(commandBuffer, currentFrame);
        }
    }
};
