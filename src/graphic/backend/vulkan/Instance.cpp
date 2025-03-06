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
    _debugMessenger = std::make_unique<DebugUtilsMessengerEXT>();
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) _debugMessenger->getCreateInfo();
    } else
        createInfo.enabledLayerCount = 0;
    if (vkCreateInstance(&createInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");
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
    _debugMessenger->setup(&_primitive);
    _window = std::make_unique<Window>(800, 600, title);
    _surface = std::make_unique<Surface>(&_primitive, _window->getPrimitive());
    _physicalDevice = std::make_unique<PhysicalDevice>(_primitive, _surface);
    _device = std::make_unique<Device>(_physicalDevice);
    _swapchain = std::make_unique<Swapchain>(*this);
    _imageViews.reserve(_swapchain->getImages().size());
    for (auto &image : _swapchain->getImages())
        _imageViews.emplace_back(_device, image, _swapchain->getFormat());
    _renderPass = std::make_unique<RenderPass>(_device, _swapchain->getFormat());
    _graphicsPipeline = std::make_unique<GraphicsPipeline>(_device, _swapchain);
    _frameBuffers = std::make_unique<FrameBuffers>(_graphicsPipeline, _device, _imageViews, _swapchain->getExtent());
    _commandPool = std::make_unique<CommandPool>(_device, _physicalDevice->getQueueFamily());
    _commandBuffer = std::make_unique<CommandBuffer>(_device, _commandPool);
    _syncObj = std::make_unique<SyncObj>(_device);
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
    std::cerr << "[" << messageType << "] " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

Instance::~Instance()
{
    this->_syncObj.reset();
    this->_commandBuffer.reset();
    this->_commandPool.reset();
    this->_frameBuffers.reset();
    this->_graphicsPipeline.reset();
    this->_renderPass.reset();
    this->_imageViews.clear();
    this->_swapchain.reset();
    this->_device.reset();
    this->_physicalDevice.reset();
    this->_surface.reset();
    this->_window.reset();
    this->_debugMessenger.reset();
    vkDestroyInstance(_primitive, nullptr);
}

std::vector<const char *> Instance::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #ifdef _WIN32
        extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #elif __linux__
        extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
    #endif
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

std::unique_ptr<PhysicalDevice> &Instance::getPhysicalDevice()
{
    return _physicalDevice;
}

std::unique_ptr<Device> &Instance::getDevice()
{
    return _device;
}

std::unique_ptr<Surface>  &Instance::getSurface()
{
    return _surface;
}

std::unique_ptr<Window> &Instance::getWindow()
{
    return _window;
}

std::unique_ptr<Swapchain> &Instance::getSwapchain()
{
    return _swapchain;
}

std::vector<ImageView> &Instance::getImageViews()
{
    return _imageViews;
}

std::unique_ptr<RenderPass> &Instance::getRenderPass()
{
    return _renderPass;
}

std::unique_ptr<GraphicsPipeline> &Instance::getGraphicsPipeline()
{
    return _graphicsPipeline;
}

std::unique_ptr<FrameBuffers> &Instance::getFrameBuffers()
{
    return _frameBuffers;
}

std::unique_ptr<CommandPool> &Instance::getCommandPool()
{
    return _commandPool;
}

std::unique_ptr<CommandBuffer> &Instance::getCommandBuffer()
{
    return _commandBuffer;
}

std::unique_ptr<SyncObj> &Instance::getSyncObj()
{
    return _syncObj;
}
