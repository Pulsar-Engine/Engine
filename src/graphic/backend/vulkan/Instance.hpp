#ifndef INSTANCE_HPP_
    #define INSTANCE_HPP_

    #include "PhysicalDevice.hpp"
    #include "DebugUtilsMessengerEXT.hpp"
    #include "Device.hpp"
    #include "Surface.hpp"
    #include "../../Window.hpp"
    #include "Swapchain.hpp"
    #include "ImageView.hpp"
    #include "render/RenderPass.hpp"
    #include "render/GraphicsPipeline.hpp"
    #include "render/FrameBuffers.hpp"
    #include "CommandBuffer.hpp"
    #include "SyncObj.hpp"

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
    const bool enableValidationLayers = true;
#endif

class Instance : public Primitive<VkInstance> {
    public:
        Instance(const char *title);
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
        std::unique_ptr<Window> &getWindow();
        std::unique_ptr<Swapchain> &getSwapchain();
        std::vector<ImageView> &getImageViews();
        std::unique_ptr<RenderPass> &getRenderPass();
        std::unique_ptr<GraphicsPipeline> &getGraphicsPipeline();
        std::unique_ptr<FrameBuffers> &getFrameBuffers();
        std::unique_ptr<CommandPool> &getCommandPool();
        std::unique_ptr<CommandBuffer> &getCommandBuffer();
        std::unique_ptr<SyncObj> &getSyncObj();
    protected:
    private:
        std::unique_ptr<PhysicalDevice> _physicalDevice;
        std::unique_ptr<Device> _device;
        std::unique_ptr<Surface> _surface;
        std::unique_ptr<DebugUtilsMessengerEXT> _debugMessenger;
        std::unique_ptr<Window> _window;
        std::unique_ptr<Swapchain> _swapchain;
        std::vector<ImageView> _imageViews;
        std::unique_ptr<RenderPass> _renderPass;
        std::unique_ptr<GraphicsPipeline> _graphicsPipeline;
        std::unique_ptr<FrameBuffers> _frameBuffers;
        std::unique_ptr<CommandPool> _commandPool;
        std::unique_ptr<CommandBuffer> _commandBuffer;
        std::unique_ptr<SyncObj> _syncObj;
};

#endif
