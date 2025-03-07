#ifndef DESCRIPTORPOOL_HPP_
    #define DESCRIPTORPOOL_HPP_

    #include "Primitive.hpp"
    #include "Device.hpp"
    #include "DescriptorSetLayout.hpp"

class DescriptorPool : public Primitive<VkDescriptorPool> {
    public:
        DescriptorPool(std::unique_ptr<Device> &device);
        ~DescriptorPool();
    protected:
    private:
        std::unique_ptr<Device> &_device;
};

#endif
