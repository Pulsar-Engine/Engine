#ifndef INSTANCE_HPP_
    #define INSTANCE_HPP_

    #include <vulkan/vulkan.h>
    #include <vulkan/vulkan_core.h>
    #include <GLFW/glfw3.h>
    #include <stdexcept>
    #include <vector>
    #include <iostream>
    #include <optional>

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class Instance {
    public:
        Instance(const char *title);
        ~Instance();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
    protected:
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        void setupDebugMessenger();
        VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    private:
        VkInstance primitiveInstance;
        VkDebugUtilsMessengerEXT debugMessenger;
};

#endif /* !INSTANCE_HPP_ */
