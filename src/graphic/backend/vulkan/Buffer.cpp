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

    _oldMemory = VK_NULL_HANDLE;

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
    if (_oldMemory != VK_NULL_HANDLE)
        vkFreeMemory(_device->getPrimitive(), _oldMemory, nullptr);
}

void Buffer::bindImage(Instance &instance, VkImage &image, VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device->getPrimitive(), image, &memRequirements);
    _oldMemory = _memory;
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = instance.findMemoryType(memRequirements.memoryTypeBits, properties);
    if (vkAllocateMemory(_device->getPrimitive(), &allocInfo, nullptr, &_memory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    vkBindImageMemory(_device->getPrimitive(), image, _memory, 0);
}



void Buffer::copyTo(std::unique_ptr<CommandPool> &commandPool, Buffer &dstBuffer)
{
    VkCommandBuffer commandBuffer = CommandBuffers::beginSingleTimeCommands(_device, commandPool);
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = _size;
    vkCmdCopyBuffer(commandBuffer, _primitive, dstBuffer.getPrimitive(), 1, &copyRegion);
    CommandBuffers::endSingleTimeCommands(_device, commandPool, commandBuffer);
    dstBuffer._data = _data;
}

VkDeviceSize Buffer::getSize()
{
    return _size;
}

void Buffer::map()
{
    vkMapMemory(_device->getPrimitive(), _memory, 0, _size, 0, &_data);
}

void Buffer::copyData(void *data)
{
    memcpy(_data, data, (size_t) _size);
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

void Buffer::copyToImage(std::unique_ptr<CommandPool> &commandPool, Image &image)
{
    VkCommandBuffer commandBuffer = CommandBuffers::beginSingleTimeCommands(_device, commandPool);
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent.width = image.getWidth();
    region.imageExtent.height = image.getHeight();
    region.imageExtent.depth = 1;
    vkCmdCopyBufferToImage(commandBuffer, _primitive, image.getPrimitive(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    CommandBuffers::endSingleTimeCommands(_device, commandPool, commandBuffer);
    if (_oldMemory != VK_NULL_HANDLE) {
        vkFreeMemory(_device->getPrimitive(), _oldMemory, nullptr);
        _oldMemory = VK_NULL_HANDLE;
    }
}