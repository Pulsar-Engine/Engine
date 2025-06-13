#ifndef COMMANDPool_HPP_
    #define COMMANDPool_HPP_

    #include "Primitive.hpp"
    #include "Device.hpp"
    #include "PhysicalDevice.hpp"

class CommandPool : public Primitive<VkCommandPool> {
    public:
        CommandPool(std::unique_ptr<Device> &device, QueueFamilyIndices &queueFamilyIndices);
        ~CommandPool();
    protected:
    private:
        std::unique_ptr<Device> &_device;
};

#endif
