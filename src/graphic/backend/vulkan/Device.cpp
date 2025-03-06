
#include "Device.hpp"

Device::Device(std::unique_ptr<PhysicalDevice> &physicalDevice) : _physicalDevice(physicalDevice)
{
    QueueFamilyIndices indices = _physicalDevice->getQueueFamily();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    if (vkCreateDevice(_physicalDevice->getPrimitive(), &createInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");
    vkGetDeviceQueue(_primitive, indices.graphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_primitive, indices.presentFamily.value(), 0, &_presentQueue);

}

Device::~Device()
{
    if (_primitive == VK_NULL_HANDLE)
        return;
    vkDestroyDevice(_primitive, nullptr);
}

VkQueue &Device::getGraphicsQueue()
{
    return _graphicsQueue;
}

void Device::waitIdle()
{
    vkDeviceWaitIdle(_primitive);
}

VkQueue &Device::getPresentQueue()
{
    return _presentQueue;
}

