#pragma once

#include <memory>
#include "src/graphic/backend/vulkan/render/Mesh.hpp"

struct MeshComponent {
    std::shared_ptr<Mesh> mesh;

    MeshComponent() = default;

    MeshComponent(std::shared_ptr<Mesh> meshPtr)
        : mesh(std::move(meshPtr)) {}
};
