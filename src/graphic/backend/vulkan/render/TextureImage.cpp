#include "TextureImage.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TextureImage::TextureImage(const char *path)
{
    _pixels = stbi_load(path, &_width, &_height, &_channels, STBI_rgb_alpha);
    if (!_pixels)
        throw std::runtime_error("failed to load texture image!");
}

TextureImage::~TextureImage()
{
    stbi_image_free(_pixels);
}

int TextureImage::getWidth() const
{
    return _width;
}

int TextureImage::getHeight() const
{
    return _height;
}

int TextureImage::getChannels() const
{
    return _channels;
}

stbi_uc * TextureImage::getPixels() const
{
    return _pixels;
}

VkDeviceSize TextureImage::getSize() const
{
    return _width * _height * _channels;
}

