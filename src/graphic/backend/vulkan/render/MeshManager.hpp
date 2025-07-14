#ifndef MESH_MANAGER_HPP_
#define MESH_MANAGER_HPP_

#include "Mesh.hpp"
#include <vector>
#include <memory>

class Instance;

class MeshManager {
    public:
        MeshManager();
        ~MeshManager();
        
        void addMesh(Instance &instance, const char *modelPath, const char *texturePath);
        void addMesh(Instance &instance, const char *modelPath, const char *texturePath, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
        void removeMesh(size_t index);
        void clear();
        
        size_t getMeshCount() const;
        Mesh* getMesh(size_t index);
        const std::vector<std::unique_ptr<Mesh>>& getAllMeshes() const;
        
        void updateUniformBuffers(uint32_t currentFrame, const glm::mat4& view, const glm::mat4& proj);
        void bindAndDraw(VkCommandBuffer commandBuffer, uint32_t currentFrame);
        
    protected:
    private:
        std::vector<std::unique_ptr<Mesh>> _meshes;
};

#endif /* !MESH_MANAGER_HPP_ */
