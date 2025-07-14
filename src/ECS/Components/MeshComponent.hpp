#pragma once

#include <memory>
#include "graphic/backend/vulkan/render/Mesh.hpp"

struct MeshComponent {
    std::shared_ptr<Mesh> mesh;

    MeshComponent() = default;

    MeshComponent(std::shared_ptr<Mesh> &meshPtr)
        : mesh(meshPtr) {}
};
