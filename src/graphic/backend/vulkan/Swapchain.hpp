
#ifndef SWAPCHAIN_HPP_
    #define SWAPCHAIN_HPP_

#include "Instance.hpp"

#include <algorithm> 
#include <cstdint>
#include <limits>

class Swapchain : public Primitive<VkSwapchainKHR> {
    public:
        Swapchain() = default;
        Swapchain(Instance &instace);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(Instance &instance, const VkSurfaceCapabilitiesKHR& capabilities);
        ~Swapchain();
        Swapchain &operator=(const Swapchain &rvalue);
    protected:
    private:
        Device *_device;
};

#endif /* !SWAPCHAIN_HPP_ */
