#ifndef MESH_HPP_
#define MESH_HPP_

#include "../Buffer.hpp"
#include "../ImageView.hpp"
#include "Model.hpp"
#include "../DescriptorSets.hpp"
#include "UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Instance;

class Mesh {
    public:
        Mesh(Instance &instance, const char *modelPath, const char *texturePath);
        ~Mesh();
        
        // Transformation methods
        void setPosition(const glm::vec3& position);
        void setRotation(const glm::vec3& rotation);
        void setScale(const glm::vec3& scale);
        
        glm::vec3 getPosition() const;
        glm::vec3 getRotation() const;
        glm::vec3 getScale() const;
        
        glm::mat4 getModelMatrix() const;
        
        // Rendering methods
        void updateUniformBuffer(uint32_t currentFrame, const glm::mat4& view, const glm::mat4& proj);
        void bindAndDraw(VkCommandBuffer commandBuffer, uint32_t currentFrame);
        
        // Getters
        const std::vector<Vertex>& getVertices() const;
        const std::vector<uint32_t>& getIndices() const;
        
    protected:
    private:
        // Model and rendering data
        std::unique_ptr<Model> _model;
        std::unique_ptr<Buffer> _vertexBuffer;
        std::unique_ptr<Buffer> _indexBuffer;
        std::unique_ptr<Buffer> _stagingBuffer;
        std::unique_ptr<Image> _image;
        std::unique_ptr<ImageView> _imageView;
        std::unique_ptr<DescriptorSets> _descriptorSets;
        std::unique_ptr<DescriptorPool> _descriptorPool;
        std::vector<std::unique_ptr<Buffer>> _uniformBuffers;
        
        // Transformation data
        glm::vec3 _position;
        glm::vec3 _rotation;
        glm::vec3 _scale;
        
        // Instance reference
        Instance* _instance;
};

#endif /* !MESH_HPP_ */
