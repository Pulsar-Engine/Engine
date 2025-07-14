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
    
    // Initialize mesh manager
    _meshManager = std::make_unique<MeshManager>();
    
    // Add a default mesh (viking room)
    // Note: This will be replaced by the user adding their own meshes
    
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
    
    // Add multiple meshes for testing
    try {
        // Pour le moment, utilisons le fallback car le système Mesh a besoin d'adjustements
        createBuffers();
        
        // TODO: Activer le système MeshManager une fois tous les bugs corrigés
        // addMesh("models/viking_room.obj", "textures/viking_room.png");
        // addMesh("models/viking_room.obj", "textures/viking_room.png", glm::vec3(2.0f, 0.0f, 0.0f));
        // addMesh("models/viking_room.obj", "textures/viking_room.png", glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 45.0f, 0.0f));
    } catch (const std::exception& e) {
        std::cerr << "Error adding mesh: " << e.what() << std::endl;
        // For now, fall back to the old system if mesh loading fails
        createBuffers();
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

// Fallback getters for compatibility
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
    // Load the base model
    Model model("models/viking_room.obj");
    const std::vector<Vertex> &baseVertices = model.getVertices();
    const std::vector<uint32_t> &baseIndices = model.getIndices();
    
    // Create multiple instances by duplicating vertices with different positions
    _vertices.clear();
    _indices.clear();
    
    // Position offsets for multiple instances
    std::vector<glm::vec3> positions = {
        glm::vec3(0.0f, 0.0f, 0.0f),    // Center
        glm::vec3(2.0f, 0.0f, 0.0f),    // Right
        glm::vec3(-2.0f, 0.0f, 0.0f)    // Left
    };
    
    for (size_t i = 0; i < positions.size(); ++i) {
        uint32_t vertexOffset = static_cast<uint32_t>(_vertices.size());
        
        // Add vertices with position offset
        for (const auto& vertex : baseVertices) {
            Vertex newVertex = vertex;
            newVertex.pos += positions[i];
            _vertices.push_back(newVertex);
        }
        
        // Add indices with vertex offset
        for (const auto& index : baseIndices) {
            _indices.push_back(index + vertexOffset);
        }
    }
    
    _vertexBuffer = std::make_unique<Buffer>(
        *this,
        sizeof(_vertices[0]) * _vertices.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    _vertexBuffer->CPUToGPU(*this, (void *) _vertices.data());
    
    _indexBuffer = std::make_unique<Buffer>(
        *this,
        sizeof(_indices[0]) * _indices.size(),
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    _indexBuffer->CPUToGPU(*this, (void *) _indices.data());
    
    TextureImage texture("textures/viking_room.png");
    _stagingBuffer = std::make_unique<Buffer>(
        *this,
        texture.getSize(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    _stagingBuffer->mapTo(texture.getPixels());
    texture.freePixels();
    
    _image = std::make_unique<Image>(
        *this,
        texture,
        *_device,
        *_stagingBuffer,
        _swapchain->getFormat(),
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    
    _textureImageView = std::make_unique<ImageView>(
        _device,
        *_image,
        _swapchain->getFormat(),
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    
    _uniformBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _uniformBuffers.emplace_back(
            *this,
            sizeof(UniformBufferObject),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        _uniformBuffers[i].map();
        
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = _uniformBuffers[i].getPrimitive();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = _textureSampler->getPrimitive();
        imageInfo.imageView = _textureImageView->getPrimitive();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        _descriptorSets->write(i, bufferInfo, imageInfo);
    }
    
    _commandBuffers->transitionImageLayout(*_image, 
        VK_IMAGE_LAYOUT_UNDEFINED, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    _stagingBuffer->copyToImage(_commandPool, *_image);
    _commandBuffers->transitionImageLayout(*_image, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}