#include "Image.hpp"
#include "../Instance.hpp"

Image::Image(Instance &instance,
    TextureImage &textureImage,
    Device &device,
    Buffer &stagingBuffer,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties) : Image(instance, textureImage.getWidth(), textureImage.getHeight(), device, stagingBuffer, format, tiling, usage, properties)
{

}

Image::Image(Instance &instance,
    int width,
    int height,
    Device &device,
    Buffer &stagingBuffer,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties)
    : _device(device), _width(width), _height(height), _format(format)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = _width;
    imageInfo.extent.height = _height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(_device.getPrimitive(), &imageInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create texture image!");

    stagingBuffer.bindImage(instance, _primitive, properties);
}

Image::~Image()
{
    vkDestroyImage(_device.getPrimitive(), _primitive, nullptr);
}

int Image::getWidth() const
{
    return _width;
}

int Image::getHeight() const
{
    return _height;
}

VkFormat Image::getFormat() const
{
    return _format;
}
