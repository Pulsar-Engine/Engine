
#ifndef PHYSICALDEVICE_HPP_
    #define PHYSICALDEVICE_HPP_

#include <vector>
#include <optional>
#include <set>

#include "Primitive.hpp"
#include "Surface.hpp"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class PhysicalDevice : public Primitive<VkPhysicalDevice> {
    public:
        PhysicalDevice() = default;
        PhysicalDevice(VkInstance &instance, std::unique_ptr<Surface> &surface);
        ~PhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        QueueFamilyIndices &getQueueFamily();
        std::unique_ptr<Surface> &getSurface();
    protected:
    private:
        QueueFamilyIndices _indices;
        std::unique_ptr<Surface> &_surface;
};

#endif
