#include "DescriptorPool.hpp"

DescriptorPool::DescriptorPool(std::unique_ptr<Device> &device) : _device(device)
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    if (vkCreateDescriptorPool(_device->getPrimitive(), &poolInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");
}

DescriptorPool::~DescriptorPool()
{
    if (_primitive == VK_NULL_HANDLE)
        return;
    vkDestroyDescriptorPool(_device->getPrimitive(), _primitive, nullptr);
}

