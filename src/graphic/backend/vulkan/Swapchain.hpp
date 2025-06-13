
#ifndef SWAPCHAIN_HPP_
    #define SWAPCHAIN_HPP_

#include "Primitive.hpp"

#include <algorithm> 
#include <cstdint>
#include <limits>
#include <vector>

class Instance;
class Device;

class Swapchain : public Primitive<VkSwapchainKHR> {
    public:
        Swapchain() = default;
        Swapchain(Instance &instace);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(Instance &instance, const VkSurfaceCapabilitiesKHR& capabilities);
        ~Swapchain();
        std::vector<VkImage> &getImages();
        VkFormat &getFormat();
        VkExtent2D &getExtent();
    protected:
    private:
        std::unique_ptr<Device> &_device;
        std::vector<VkImage> _images;
        VkFormat _imageFormat;
        VkExtent2D _imageExtent;
};

#endif /* !SWAPCHAIN_HPP_ */
