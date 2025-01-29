#ifndef DEVICE_HPP_
    #define DEVICE_HPP_

    #include "PhysicalDevice.hpp"
    
    #include <set>

class Device : public Primitive<VkDevice> {
    public:
        Device() = default;
        Device(PhysicalDevice *physicalDevice);
        ~Device();
        Device &operator=(const Device &rvalue);
    protected:
    private:
        PhysicalDevice *_physicalDevice;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;
};

#endif
