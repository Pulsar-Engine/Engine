#ifndef IMAGE_HPP_
    #define IMAGE_HPP_

    #include "../Primitive.hpp"
    #include "TextureImage.hpp"
    #include "../Device.hpp"
    #include "../Buffer.hpp"

class Image : public Primitive<VkImage> {
    public:
        Image(Instance &instance,
            TextureImage &textureImage,
            Device &device,
            Buffer &stagingBuffer,
            VkFormat format,
            VkImageTiling tiling,
            VkImageUsageFlags usage,
            VkMemoryPropertyFlags properties);
        ~Image();
    protected:
    private:
        Device &_device;
};

#endif /* !IMAGE_HPP_ */
