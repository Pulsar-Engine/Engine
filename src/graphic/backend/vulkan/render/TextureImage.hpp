#ifndef TEXTUREIMAGE_HPP_
    #define TEXTUREIMAGE_HPP_

    #include <vulkan/vulkan.h>
    #include <stdexcept>

typedef unsigned char stbi_uc;

class TextureImage {
    public:
        TextureImage(const char *path);
        ~TextureImage();
        int getWidth() const;
        int getHeight() const;
        int getChannels() const;
        stbi_uc * getPixels() const;
        VkDeviceSize getSize() const;
        void freePixels();
    protected:
    private:
        int _width;
        int _height;
        int _channels;
        stbi_uc * _pixels;
};

#endif /* !TEXTUREIMAGE_HPP_ */
