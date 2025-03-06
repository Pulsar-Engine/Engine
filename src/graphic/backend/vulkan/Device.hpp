#ifndef DEVICE_HPP_
    #define DEVICE_HPP_

    #include "PhysicalDevice.hpp"
    
    #include <set>

class Device : public Primitive<VkDevice> {
    public:
        Device() = default;
        Device(std::unique_ptr<PhysicalDevice> &physicalDevice);
        ~Device();
        VkQueue &getGraphicsQueue();
        VkQueue &getPresentQueue();
        void waitIdle();
    protected:
    private:
        std::unique_ptr<PhysicalDevice> &_physicalDevice;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;
};

#endif
