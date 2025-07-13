#pragma once

#include <glm/glm.hpp>

struct CameraComponent {
    glm::vec3 position{0.0f, 0.0f, 3.0f};
    glm::vec3 target{0.0f, 0.0f, 0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    float fov{45.0f};
    float nearClip{0.1f};
    float farClip{100.0f};

    CameraComponent() = default;

    CameraComponent(const glm::vec3& pos, const glm::vec3& tgt, const glm::vec3& upVec,
                    float fovDeg, float nearZ, float farZ)
        : position(pos), target(tgt), up(upVec), fov(fovDeg), nearClip(nearZ), farClip(farZ) {}
};
