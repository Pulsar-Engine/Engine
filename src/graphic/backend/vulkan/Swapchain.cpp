#include "Swapchain.hpp"
#include "Instance.hpp"

Swapchain::Swapchain(Instance &instance) : _device(instance.getDevice())
{
    std::unique_ptr<PhysicalDevice> &physicalDevice = instance.getPhysicalDevice();
    SwapChainSupportDetails swapChainSupport = physicalDevice->querySwapChainSupport(physicalDevice->getPrimitive());
    QueueFamilyIndices indices = physicalDevice->getQueueFamily();
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    _imageExtent = chooseSwapExtent(instance, swapChainSupport.capabilities);
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = physicalDevice->getSurface()->getPrimitive();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = _imageExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(_device->getPrimitive(), &createInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create swap chain!");
    vkGetSwapchainImagesKHR(_device->getPrimitive(), _primitive, &imageCount, nullptr);
    _images.resize(imageCount);
    vkGetSwapchainImagesKHR(_device->getPrimitive(), _primitive, &imageCount, _images.data());
    _imageFormat = surfaceFormat.format;

}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }
    return availableFormats[0];
}

VkExtent2D Swapchain::chooseSwapExtent(Instance &instance, const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
        return capabilities.currentExtent;
    int width;
    int height;
    glfwGetFramebufferSize(instance.getWindow()->getPrimitive(), &width, &height);
    VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    actualExtent.width = (std::max)(capabilities.minImageExtent.width, (std::min)(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = (std::max)(capabilities.minImageExtent.height, (std::min)(capabilities.maxImageExtent.height, actualExtent.height));
    return actualExtent;
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

Swapchain::~Swapchain()
{
    if (_primitive == VK_NULL_HANDLE)
        return;
    vkDestroySwapchainKHR(_device->getPrimitive(), _primitive, nullptr);
}

std::vector<VkImage> &Swapchain::getImages()
{
    return _images;
}

VkFormat &Swapchain::getFormat()
{
    return _imageFormat;
}

VkExtent2D &Swapchain::getExtent()
{
    return _imageExtent;
}

