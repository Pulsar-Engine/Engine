#ifndef IMAGEVIEW_HPP_
    #define IMAGEVIEW_HPP_

    #include "Primitive.hpp"
    #include "Device.hpp"

    #include <vector>

class Image;

class ImageView : public Primitive<VkImageView> {
    public:
        ImageView(std::unique_ptr<Device> &device, VkImage &image, VkFormat format, VkImageAspectFlags aspectFlags);
        ImageView(std::unique_ptr<Device> &device, Image &image, VkFormat format, VkImageAspectFlags aspectFlags);
        ~ImageView();
    protected:
    private:
        std::unique_ptr<Device> &_device;
};

#endif
