#include "MeshManager.hpp"
#include "../Instance.hpp"

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
    clear();
}

void MeshManager::addMesh(Instance &instance, const char *modelPath, const char *texturePath)
{
    _meshes.emplace_back(std::make_unique<Mesh>(instance, modelPath, texturePath));
}

void MeshManager::addMesh(Instance &instance, const char *modelPath, const char *texturePath, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    auto mesh = std::make_unique<Mesh>(instance, modelPath, texturePath);
    mesh->setPosition(position);
    mesh->setRotation(rotation);
    mesh->setScale(scale);
    _meshes.emplace_back(std::move(mesh));
}

void MeshManager::removeMesh(size_t index)
{
    if (index < _meshes.size()) {
        _meshes.erase(_meshes.begin() + index);
    }
}

void MeshManager::clear()
{
    _meshes.clear();
}

size_t MeshManager::getMeshCount() const
{
    return _meshes.size();
}

Mesh* MeshManager::getMesh(size_t index)
{
    if (index < _meshes.size()) {
        return _meshes[index].get();
    }
    return nullptr;
}

std::vector<std::unique_ptr<Mesh>>& MeshManager::getAllMeshes()
{
    return _meshes;
}

void MeshManager::updateUniformBuffers(uint32_t currentFrame, const glm::mat4& view, const glm::mat4& proj)
{
    for (auto& mesh : _meshes) {
        mesh->updateUniformBuffer(currentFrame, view, proj);
    }
}

void MeshManager::bindAndDraw(VkCommandBuffer commandBuffer, uint32_t currentFrame)
{
    for (auto& mesh : _meshes) {
        mesh->bindAndDraw(commandBuffer, currentFrame);
    }
}
