#ifndef INSTANCE_HPP_
    #define INSTANCE_HPP_

    #include "PhysicalDevice.hpp"
    #include "DebugUtilsMessengerEXT.hpp"
    #include "Device.hpp"
    #include "Surface.hpp"
    #include "../../GWindow.hpp"
    #include "Swapchain.hpp"
    #include "ImageView.hpp"
    #include "render/RenderPass.hpp"
    #include "render/GraphicsPipeline.hpp"
    #include "render/FrameBuffers.hpp"
    #include "CommandBuffers.hpp"
    #include "SyncObj.hpp"
    #include "render/Vertex.hpp"
    #include "Buffer.hpp"
    #include "DescriptorSetLayout.hpp"
    #include "render/UniformBufferObject.hpp"
    #include "DescriptorPool.hpp"
    #include "DescriptorSets.hpp"
    #include "render/TextureImage.hpp"
    #include "render/TextureSampler.hpp"
    #include "render/DepthResources.hpp"
    #include "render/MeshManager.hpp"

    #include <memory>
    #include <GLFW/glfw3.h>
    #include <cstring>
    #include <iostream>
    #include <cstdlib>

    #ifdef _WIN32
        #include <vulkan/vulkan_win32.h>
    #elif __linux__
        #include <xcb/xcb.h>
        #include <vulkan/vulkan_xcb.h>
    #endif

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    extern bool enableValidationLayers;
#endif

class Instance : public Primitive<VkInstance> {
    public:
        Instance(const char *title, bool fromEditor = false);
        ~Instance();
        bool checkValidationLayerSupport();
        std::vector<const char *> getRequiredExtensions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);
        std::unique_ptr<PhysicalDevice> &getPhysicalDevice();
        std::unique_ptr<Device> &getDevice();
        std::unique_ptr<Surface> &getSurface();
        std::unique_ptr<GWindow> &getWindow();
        std::unique_ptr<Swapchain> &getSwapchain();
        std::vector<ImageView> &getImageViews();
        std::unique_ptr<DescriptorSetLayout> &getDescriptorSetLayout();
        std::unique_ptr<GraphicsPipeline> &getGraphicsPipeline();
        std::unique_ptr<FrameBuffers> &getFrameBuffers();
        std::unique_ptr<CommandPool> &getCommandPool();
        std::unique_ptr<CommandBuffers> &getCommandBuffers();
        std::unique_ptr<DepthResources> &getDepthResources();
        std::unique_ptr<DescriptorPool> &getDescriptorPool();
        std::unique_ptr<DescriptorSets> &getDescriptorSets();
        std::unique_ptr<TextureSampler> &getTextureSampler();
        
        std::unique_ptr<Buffer> &getVertexBuffer();
        std::vector<Vertex> &getVertices();
        std::vector<uint32_t> &getIndices();
        std::unique_ptr<Buffer> &getIndexBuffer();
        std::vector<Buffer> &getUniformBuffers();
        std::unique_ptr<Image> &getImage();
        
        void recreateSwapchain();
        void cleanupSwapchain();
        void createBuffers();
        void createCommandBuffers();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        // Mesh management
        MeshManager& getMeshManager();
        void addMesh(const char *modelPath, const char *texturePath);
        void addMesh(const char *modelPath, const char *texturePath, glm::vec3 position, glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
    protected:
    private:
        std::unique_ptr<PhysicalDevice> _physicalDevice;
        std::unique_ptr<Device> _device;
        std::unique_ptr<Surface> _surface;
        std::unique_ptr<DebugUtilsMessengerEXT> _debugMessenger;
        std::unique_ptr<GWindow> _window;
        std::unique_ptr<Swapchain> _swapchain;
        std::vector<ImageView> _imageViews;
        std::unique_ptr<DescriptorSetLayout> _descriptorSetLayout;
        std::unique_ptr<GraphicsPipeline> _graphicsPipeline;
        std::unique_ptr<FrameBuffers> _frameBuffers;
        std::unique_ptr<CommandPool> _commandPool;
        std::unique_ptr<CommandBuffers> _commandBuffers;
        std::unique_ptr<DescriptorPool> _descriptorPool;
        std::unique_ptr<DescriptorSets> _descriptorSets;
        std::unique_ptr<TextureSampler> _textureSampler;
        std::unique_ptr<DepthResources> _depthResources;
        
        // Fallback variables for compatibility
        std::unique_ptr<Buffer> _vertexBuffer;
        std::unique_ptr<Buffer> _indexBuffer;
        std::unique_ptr<Buffer> _stagingBuffer;
        std::vector<Buffer> _uniformBuffers;
        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;
        std::unique_ptr<Image> _image;
        std::unique_ptr<ImageView> _textureImageView;
        
        // Mesh management
        std::unique_ptr<MeshManager> _meshManager;
};

#endif
