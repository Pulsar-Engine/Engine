#include "DescriptorPool.hpp"

DescriptorPool::DescriptorPool(std::unique_ptr<Device> &device) : _device(device)
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
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

