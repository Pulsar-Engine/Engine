#include "Buffer.hpp"
#include "Instance.hpp"
#include "CommandPool.hpp"

Buffer::Buffer(Instance &instance, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) : _device(instance.getDevice()), _size(size)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = _size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(_device->getPrimitive(), &bufferInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create vertex buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device->getPrimitive(), _primitive, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = instance.findMemoryType(memRequirements.memoryTypeBits, properties);
    
    if (vkAllocateMemory(_device->getPrimitive(), &allocInfo, nullptr, &_memory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    
    vkBindBufferMemory(_device->getPrimitive(), _primitive, _memory, 0);
}

Buffer::~Buffer()
{
    if (_primitive == VK_NULL_HANDLE)
        return;
    vkDestroyBuffer(_device->getPrimitive(), _primitive, nullptr);
    vkFreeMemory(_device->getPrimitive(), _memory, nullptr);
}

void Buffer::copyTo(std::unique_ptr<CommandPool> &commandPool, Buffer &dstBuffer)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool->getPrimitive();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device->getPrimitive(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = _size;
    vkCmdCopyBuffer(commandBuffer, _primitive, dstBuffer.getPrimitive(), 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(_device->getPresentQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_device->getPresentQueue());
    vkFreeCommandBuffers(_device->getPrimitive(), commandPool->getPrimitive(), 1, &commandBuffer);
    dstBuffer._data = _data;
}

VkDeviceSize Buffer::getSize()
{
    return _size;
}

void Buffer::mapTo(void *data)
{
    vkMapMemory(_device->getPrimitive(), _memory, 0, _size, 0, &_data);
    memcpy(_data, data, (size_t) _size);
    vkUnmapMemory(_device->getPrimitive(), _memory);
}

void Buffer::CPUToGPU(Instance &instance, void *data)
{
    Buffer stagingBuffer(
        instance,
        _size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    stagingBuffer.mapTo(data);
    stagingBuffer.copyTo(instance.getCommandPool(), *this);
}