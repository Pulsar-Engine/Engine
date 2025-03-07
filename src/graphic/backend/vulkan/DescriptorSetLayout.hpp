#ifndef DESCRIPTORSETLAYOUT_HPP_
    #define DESCRIPTORSETLAYOUT_HPP_

    #include "Primitive.hpp"
    #include "Device.hpp"

class DescriptorSetLayout : public Primitive<VkDescriptorSetLayout> {
    public:
        DescriptorSetLayout(std::unique_ptr<Device> &device);
        ~DescriptorSetLayout();
        
    protected:
    private:
        std::unique_ptr<Device> &_device;
};

#endif
