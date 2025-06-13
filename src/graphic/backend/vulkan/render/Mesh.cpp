#include "Mesh.hpp"
#include "../Instance.hpp"

Mesh::Mesh(Instance& instance)
    : _instance(instance) {
}

void Mesh::draw(VkCommandBuffer cmd, VkPipelineLayout layout, uint32_t currentFrame) {
    VkBuffer vBuffers[] = { vertexBuffer->getPrimitive() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, vBuffers, offsets);
    vkCmdBindIndexBuffer(cmd, indexBuffer->getPrimitive(), 0, VK_INDEX_TYPE_UINT32);
    
    vkCmdBindDescriptorSets(
        cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        layout,
        0, 1, &descriptorSets->getPrimitive()[currentFrame],
        0, nullptr
    );
    
    vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}
