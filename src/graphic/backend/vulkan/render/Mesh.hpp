#ifndef MESH_HPP_
    #define MESH_HPP_

    #include "../CommandBuffers.hpp"
    #include "UniformBufferObject.hpp"
    #include "Model.hpp"
    #include "../Buffer.hpp"
    #include "../ImageView.hpp"
    #include "../DescriptorSets.hpp"

class Instance;

class Mesh {
    public:
        Mesh(Instance& instance);
        
        void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t currentFrame);
        void updateUniformBuffer(uint32_t currentImage, const UniformBufferObject& ubo);

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        std::unique_ptr<Image> textureImage;
        std::unique_ptr<ImageView> textureImageView;
        std::vector<Buffer> uniformBuffers;

    private:
        Instance& _instance;
        std::unique_ptr<DescriptorSets> descriptorSets;
};

#endif /* !MESH_HPP_ */
