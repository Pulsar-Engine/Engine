#ifndef INSTANCE_HPP_
    #define INSTANCE_HPP_

    #include "PhysicalDevice.hpp"
    #include "DebugUtilsMessengerEXT.hpp"
    #include "Device.hpp"
    #include "Surface.hpp"
    #include "../../Window.hpp"
    #include "Swapchain.hpp"
    #include "ImageView.hpp"

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
    protected:
    private:
        std::unique_ptr<PhysicalDevice> _physicalDevice;
        std::unique_ptr<Device> _device;
        std::unique_ptr<Surface> _surface;
        std::unique_ptr<DebugUtilsMessengerEXT> _debugMessenger;
        std::unique_ptr<Window> _window;
        std::unique_ptr<Swapchain> _swapchain;
        std::vector<ImageView> _imageViews;
};

#endif
