#ifndef TEXTURESAMPLER_HPP_
#define TEXTURESAMPLER_HPP_

#include "../Primitive.hpp"
#include "../Device.hpp"

class TextureSampler : public Primitive<VkSampler> {
    public:
        TextureSampler(std::unique_ptr<Device> &device, PhysicalDevice &physicalDevice);
        ~TextureSampler();
    protected:
    private:
        std::unique_ptr<Device> &_device;
};

#endif /* !TEXTURESAMPLER_HPP_ */
