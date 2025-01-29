#ifndef INSTANCE_HPP_
    #define INSTANCE_HPP_

    #include "PhysicalDevice.hpp"
    #include "DebugUtilsMessengerEXT.hpp"
    #include "Device.hpp"
    #include "Surface.hpp"
    #include "../../Window.hpp"

    #include <GLFW/glfw3.h>
    #include <iostream>
    #include <cstring>

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
        PhysicalDevice &getPhysicalDevice();
        Device &getDevice();
        Surface &getSurface();
        Window &getWindow();
    protected:
    private:
        PhysicalDevice _physicalDevice;
        Device _device;
        Surface _surface;
        DebugUtilsMessengerEXT _debugMessenger;
        Window _window;
};

#endif
