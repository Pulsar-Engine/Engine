/*
** EPITECH PROJECT, 2025
** Engine
** File description:
** DepthResources
*/

#include "DepthResources.hpp"
#include "../Instance.hpp"

DepthResources::DepthResources(Instance &instance, PhysicalDevice &physicalDevice, Swapchain &swapchain) {
     VkFormat format = findDepthFormat(physicalDevice);
    _buffer = std::make_unique<Buffer>(
        instance,
        swapchain.getExtent().width * swapchain.getExtent().height * 4,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    _image = std::make_unique<Image>(
        instance,
        swapchain.getExtent().width,
        swapchain.getExtent().height,
        *instance.getDevice(),
        *_buffer,
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    _imageView = std::make_unique<ImageView>(
        instance.getDevice(),
        *_image,
        format,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    instance.getCommandBuffers()->transitionImageLayout(
        *_image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    );
}

VkFormat DepthResources::findSupportedFormat(PhysicalDevice &physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice.getPrimitive(), format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }
    throw std::runtime_error("failed to find supported format!");
}

bool DepthResources::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat DepthResources::findDepthFormat(PhysicalDevice &physicalDevice) {
    return findSupportedFormat(
        physicalDevice,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

ImageView &DepthResources::getImageView()
{
    return *_imageView;
}

DepthResources::~DepthResources()
{
    if (_imageView)
        _imageView.reset();
    if (_image)
        _image.reset();
    if (_buffer)
        _buffer.reset();
}