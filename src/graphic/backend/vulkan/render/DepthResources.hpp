#ifndef DEPTHRESOURCES_HPP_
        #define DEPTHRESOURCES_HPP_

        #include <vulkan/vulkan.h>
        #include <vector>
        
        #include "../PhysicalDevice.hpp"
        #include "../ImageView.hpp"
        #include "../Buffer.hpp"
        #include "../Swapchain.hpp"

class Instance;

class DepthResources {
    public:
        DepthResources(Instance &instance, PhysicalDevice &physicalDevice, Swapchain &swapchain);
        ~DepthResources();
        static VkFormat findSupportedFormat(
            PhysicalDevice &physicalDevice,
            const std::vector<VkFormat>& candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features
        );
        static bool hasStencilComponent(VkFormat format);
        static VkFormat findDepthFormat(PhysicalDevice &physicalDevice);
        ImageView &getImageView();
    protected:
    private:
        std::unique_ptr<Image> _image;
        std::unique_ptr<Buffer> _buffer;
        std::unique_ptr<ImageView> _imageView;
};

#endif /* !DEPTHRESOURCES_HPP_ */
