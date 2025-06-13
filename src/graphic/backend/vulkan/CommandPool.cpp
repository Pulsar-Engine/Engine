#include "CommandPool.hpp"

CommandPool::CommandPool(std::unique_ptr<Device> &device, QueueFamilyIndices &queueFamilyIndices) : _device(device)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    if (vkCreateCommandPool(_device->getPrimitive(), &poolInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
}

CommandPool::~CommandPool()
{
    if (_primitive == VK_NULL_HANDLE)
        return;
    vkDestroyCommandPool(_device->getPrimitive(), _primitive, nullptr);
}

