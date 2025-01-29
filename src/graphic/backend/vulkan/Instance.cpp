#include "Instance.hpp"


Instance::Instance(const char *title)
{
    if constexpr(enableValidationLayers) {
        if (!checkValidationLayerSupport())
            throw std::runtime_error("validation layers requested, but not available!");
    }
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = title;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Pulsar";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    std::vector<const char *> requiredExtensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.pNext = nullptr;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) _debugMessenger.getCreateInfo();
    } else
        createInfo.enabledLayerCount = 0;
    if (vkCreateInstance(&createInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");
    _debugMessenger.setup(&_primitive);
    _surface = Surface(&_primitive, _window.getPrimitive());
    _physicalDevice = PhysicalDevice(_primitive, &_surface);
    _device = Device(&_physicalDevice);
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    for (const auto &extension : extensions) {
        for (uint32_t i = 0; i < requiredExtensions.size(); i++) {
            if (strcmp(extension.extensionName, requiredExtensions[i]) == 0) {
                requiredExtensions.erase(requiredExtensions.begin() + i);
                break;
            }
        }
    }
    if (requiredExtensions.size() != 0)
        throw std::runtime_error("failed to find required extensions!");
}

VkBool32 Instance::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    (void) messageSeverity;
    (void) messageType;
    (void) pUserData;
    (void) pCallbackData;
    //TODO add logging
    return VK_FALSE;
}

Instance::~Instance()
{
    vkDestroyInstance(_primitive, nullptr);
}

std::vector<const char *> Instance::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}

bool Instance::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    for (const char *layerName : validationLayers) {
        bool layerFound = false;
        for (const auto &layerProperties : availableLayers) {
            std::cout << layerProperties.layerName << " " << layerName <<  std::endl;
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            return false;
    }
    return true;
}

PhysicalDevice &Instance::getPhysicalDevice()
{
    return _physicalDevice;
}

Device &Instance::getDevice()
{
    return _device;
}

Surface &Instance::getSurface()
{
    return _surface;
}

Window &Instance::getWindow()
{
    return _window;
}
