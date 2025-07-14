#include "Instance.hpp"
#include "render/Model.hpp"
#include "render/TextureImage.hpp"
#include <iostream>

#ifdef NDEBUG
    bool enableValidationLayers = false;
#else
    bool enableValidationLayers = true;
#endif

Instance::Instance(const char *title, bool fromEditor)
{
    if (enableValidationLayers && !checkValidationLayerSupport())
        enableValidationLayers = false;
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
        _debugMessenger = std::make_unique<DebugUtilsMessengerEXT>();
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
    _meshManager = std::make_unique<MeshManager>();
    if (_debugMessenger.get())
        _debugMessenger->setup(&_primitive);
    _window = std::make_unique<GWindow>(800, 600, title, fromEditor);
    _surface = std::make_unique<Surface>(&_primitive, _window->getPrimitive());
    _physicalDevice = std::make_unique<PhysicalDevice>(_primitive, _surface);
    _device = std::make_unique<Device>(_physicalDevice);
    _swapchain = std::make_unique<Swapchain>(*this);
    _imageViews.reserve(_swapchain->getImages().size());
    for (auto &image : _swapchain->getImages())
        _imageViews.emplace_back(_device, image, _swapchain->getFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
    _descriptorSetLayout = std::make_unique<DescriptorSetLayout>(_device);
    _descriptorPool = std::make_unique<DescriptorPool>(_device);
    _descriptorSets = std::make_unique<DescriptorSets>(*_device, *_descriptorSetLayout, *_descriptorPool);
    _graphicsPipeline = std::make_unique<GraphicsPipeline>(*_physicalDevice, _device, _descriptorSetLayout, _swapchain);
    _commandPool = std::make_unique<CommandPool>(_device, _physicalDevice->getQueueFamily());
    _commandBuffers = std::make_unique<CommandBuffers>(_device, _commandPool);
    _depthResources = std::make_unique<DepthResources>(*this, *_physicalDevice, *_swapchain);
    _frameBuffers = std::make_unique<FrameBuffers>(_graphicsPipeline, *_depthResources, _device, _imageViews, _swapchain->getExtent());
    _textureSampler = std::make_unique<TextureSampler>(_device, *_physicalDevice);
    
    try {
        createBuffers();
    } catch (const std::exception& e) {
        std::cerr << "Error adding mesh: " << e.what() << std::endl;
    }
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
    this->_commandBuffers.reset();
    cleanupSwapchain();
    this->_meshManager.reset();
    this->_image.reset();
    this->_stagingBuffer.reset();
    this->_uniformBuffers.clear();
    this->_textureImageView.reset();
    this->_indexBuffer.reset();
    this->_vertexBuffer.reset();
    this->_textureSampler.reset();
    this->_commandPool.reset();
    this->_graphicsPipeline.reset();
    this->_descriptorPool.reset();
    this->_descriptorSetLayout.reset();
    this->_surface.reset();
    this->_window.reset();
    this->_device.reset();
    this->_physicalDevice.reset();
    if (this->_debugMessenger.get())
        this->_debugMessenger.reset();
    vkDestroyInstance(_primitive, nullptr);
}

void Instance::cleanupSwapchain()
{
    this->_frameBuffers.reset();
    this->_imageViews.clear();
    this->_swapchain.reset();
    this->_depthResources.reset();
}

void Instance::recreateSwapchain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(_window->getPrimitive(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(_window->getPrimitive(), &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(_device->getPrimitive());
    cleanupSwapchain();
    _swapchain = std::make_unique<Swapchain>(*this);
    _imageViews.reserve(_swapchain->getImages().size());
    for (auto &image : _swapchain->getImages())
        _imageViews.emplace_back(_device, image, _swapchain->getFormat(), VK_IMAGE_ASPECT_COLOR_BIT);
    _depthResources = std::make_unique<DepthResources>(*this, *_physicalDevice, *_swapchain);
    _frameBuffers = std::make_unique<FrameBuffers>(_graphicsPipeline, *_depthResources, _device, _imageViews, _swapchain->getExtent());
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

std::unique_ptr<GWindow> &Instance::getWindow()
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

std::unique_ptr<CommandBuffers> &Instance::getCommandBuffers()
{
    return _commandBuffers;
}

std::unique_ptr<DescriptorSetLayout> &Instance::getDescriptorSetLayout()
{
    return _descriptorSetLayout;
}

uint32_t Instance::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_physicalDevice->getPrimitive(), &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
            return i;
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

std::unique_ptr<DescriptorPool> &Instance::getDescriptorPool()
{
    return _descriptorPool;
}

std::unique_ptr<DescriptorSets> &Instance::getDescriptorSets()
{
    return _descriptorSets;
}

std::unique_ptr<TextureSampler> &Instance::getTextureSampler()
{
    return _textureSampler;
}

std::unique_ptr<Buffer> &Instance::getVertexBuffer()
{
    return _vertexBuffer;
}

std::vector<Vertex> &Instance::getVertices()
{
    return _vertices;
}

std::vector<uint32_t> &Instance::getIndices()
{
    return _indices;
}

std::unique_ptr<Buffer> &Instance::getIndexBuffer()
{
    return _indexBuffer;
}

std::vector<Buffer> &Instance::getUniformBuffers()
{
    return _uniformBuffers;
}

std::unique_ptr<Image> &Instance::getImage()
{
    return _image;
}

MeshManager& Instance::getMeshManager()
{
    return *_meshManager;
}

void Instance::addMesh(const char *modelPath, const char *texturePath)
{
    _meshManager->addMesh(*this, modelPath, texturePath);
}

void Instance::addMesh(const char *modelPath, const char *texturePath, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    _meshManager->addMesh(*this, modelPath, texturePath, position, rotation, scale);
}

void Instance::createBuffers()
{
    if (_meshManager->getMeshCount() == 0) {
        _meshManager->addMesh(*this, "models/viking_room.obj", "textures/viking_room.png", 
                             glm::vec3(0.0f, 0.0f, 0.0f));
    }
}