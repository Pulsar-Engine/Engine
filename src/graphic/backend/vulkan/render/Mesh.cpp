#include "Mesh.hpp"
#include "../Instance.hpp"
#include "TextureImage.hpp"

Mesh::Mesh(Instance &instance, const char *modelPath, const char *texturePath)
    : _position(0.0f), _rotation(0.0f), _scale(1.0f), _instance(&instance)
{
    _model = std::make_unique<Model>(modelPath);
    const std::vector<Vertex> &vertices = _model->getVertices();
    const std::vector<uint32_t> &indices = _model->getIndices();
    _vertexBuffer = std::make_unique<Buffer>(
        instance,
        sizeof(vertices[0]) * vertices.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    _vertexBuffer->CPUToGPU(instance, (void *) vertices.data());
    _indexBuffer = std::make_unique<Buffer>(
        instance,
        sizeof(indices[0]) * indices.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    _indexBuffer->CPUToGPU(instance, (void *) indices.data());
    TextureImage texture(texturePath);
    _stagingBuffer = std::make_unique<Buffer>(
        instance,
        texture.getSize(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    _stagingBuffer->mapTo(texture.getPixels());
    texture.freePixels();
    _image = std::make_unique<Image>(
        instance,
        texture,
        *instance.getDevice(),
        *_stagingBuffer,
        instance.getSwapchain()->getFormat(),
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    _imageView = std::make_unique<ImageView>(
        instance.getDevice(),
        *_image,
        instance.getSwapchain()->getFormat(),
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    _descriptorPool = std::make_unique<DescriptorPool>(instance.getDevice());
    _descriptorSets = std::make_unique<DescriptorSets>(*instance.getDevice(), *instance.getDescriptorSetLayout(), *_descriptorPool);
    _uniformBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _uniformBuffers.emplace_back(std::make_unique<Buffer>(
            instance,
            sizeof(UniformBufferObject),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ));
        _uniformBuffers[i]->map();
        
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = _uniformBuffers[i]->getPrimitive();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = instance.getTextureSampler()->getPrimitive();
        imageInfo.imageView = _imageView->getPrimitive();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        _descriptorSets->write(i, bufferInfo, imageInfo);
    }
    instance.getCommandBuffers()->transitionImageLayout(*_image, 
        VK_IMAGE_LAYOUT_UNDEFINED, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    _stagingBuffer->copyToImage(instance.getCommandPool(), *_image);
    instance.getCommandBuffers()->transitionImageLayout(*_image, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Mesh::~Mesh()
{
    _image.reset();
    _stagingBuffer.reset();
    _uniformBuffers.clear();
    _imageView.reset();
    _descriptorSets.reset();
    _descriptorPool.reset();
    _indexBuffer.reset();
    _vertexBuffer.reset();
    _model.reset();
}

void Mesh::setPosition(const glm::vec3& position)
{
    _position = position;
}

void Mesh::setRotation(const glm::vec3& rotation)
{
    _rotation = rotation;
}

void Mesh::setScale(const glm::vec3& scale)
{
    _scale = scale;
}

glm::vec3 Mesh::getPosition() const
{
    return _position;
}

glm::vec3 Mesh::getRotation() const
{
    return _rotation;
}

glm::vec3 Mesh::getScale() const
{
    return _scale;
}

glm::mat4 Mesh::getModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, _position);
    model = glm::rotate(model, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, _scale);
    return model;
}

void Mesh::updateUniformBuffer(uint32_t currentFrame, const glm::mat4& view, const glm::mat4& proj)
{
    UniformBufferObject ubo{};
    ubo.model = getModelMatrix();
    ubo.view = view;
    ubo.proj = proj;
    
    _uniformBuffers[currentFrame]->copyData(&ubo);
}

void Mesh::bindAndDraw(VkCommandBuffer commandBuffer, uint32_t currentFrame)
{
    VkBuffer vertexBuffers[] = { _vertexBuffer->getPrimitive() };
    VkDeviceSize offsets[] = { 0 };
    
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getPrimitive(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
                           _instance->getGraphicsPipeline()->getPipelineLayout(), 
                           0, 1, &_descriptorSets->getPrimitive()[currentFrame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(_model->getIndices().size()), 1, 0, 0, 0);
}

const std::vector<Vertex>& Mesh::getVertices() const
{
    return _model->getVertices();
}

const std::vector<uint32_t>& Mesh::getIndices() const
{
    return _model->getIndices();
}

