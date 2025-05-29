#ifndef IMAGE_HPP_
    #define IMAGE_HPP_

    #include "../Primitive.hpp"
    #include "TextureImage.hpp"
    #include "../Device.hpp"
    #include "../Buffer.hpp"

class Instance;
class Buffer;

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

        Image(Instance &instance,
            int width,
            int height,
            Device &device,
            Buffer &stagingBuffer,
            VkFormat format,
            VkImageTiling tiling,
            VkImageUsageFlags usage,
            VkMemoryPropertyFlags properties);
            
            int getWidth() const;
            int getHeight() const;
            VkFormat getFormat() const;
        ~Image();
    protected:
    private:
        Device &_device;
        int _width;
        int _height;
        VkFormat _format;
};

#endif /* !IMAGE_HPP_ */
