#include "PhysicalDevice.hpp"

PhysicalDevice::PhysicalDevice(VkInstance &instance, std::unique_ptr<Surface> &surface) : _surface(surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            _primitive = device;
            _indices = findQueueFamilies(device);
            break;
        }
    }
    if (_primitive == VK_NULL_HANDLE)
        throw std::runtime_error("failed to find a suitable GPU!");
}

QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface->getPrimitive(), &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;
        if (indices.isComplete())
            break;
        ++i;
    }
    return indices;
}



bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);
    return requiredExtensions.empty();
}

QueueFamilyIndices PhysicalDevice::getQueueFamily() {
    if (_indices.isComplete())
        return _indices;
    throw std::runtime_error("the queue family is not complete!");
}

SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    uint32_t formatCount;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface->getPrimitive(), &details.capabilities) != VK_SUCCESS)
        throw std::runtime_error("Failed to get surface capabilities!");
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface->getPrimitive(), &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface->getPrimitive(), &formatCount, details.formats.data());
    }
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface->getPrimitive(), &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface->getPrimitive(), &presentModeCount, details.presentModes.data());
    }
    return details;
}

std::unique_ptr<Surface> &PhysicalDevice::getSurface() {
    return _surface;
}

PhysicalDevice::~PhysicalDevice() {}