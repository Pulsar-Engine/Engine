#include "ImageView.hpp"
#include "Device.hpp"
#include "render/Image.hpp"

ImageView::ImageView(std::unique_ptr<Device> &device, VkImage &image, VkFormat format, VkImageAspectFlags aspectFlags) : _device(device)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(_device->getPrimitive(), &createInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create image views!");
}

ImageView::ImageView(std::unique_ptr<Device> &device, Image &image, VkFormat format, VkImageAspectFlags aspectFlags)  : _device(device)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image =  image.getPrimitive();
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(_device->getPrimitive(), &createInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create image views!");
}

ImageView::~ImageView()
{
    if (_primitive == VK_NULL_HANDLE)
        return;
    vkDestroyImageView(_device->getPrimitive(), _primitive, nullptr);
}
