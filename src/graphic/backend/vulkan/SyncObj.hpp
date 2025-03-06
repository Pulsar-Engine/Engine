
#ifndef SYNCOBJ_HPP_
    #define SYNCOBJ_HPP_

    #include "Device.hpp"
    #include "CommandBuffer.hpp"

class SyncObj {
    public:
        SyncObj(std::unique_ptr<Device> &device);
        ~SyncObj();
        VkSemaphore &getImageAvailableSemaphore();
        VkSemaphore &getRenderFinishedSemaphore();
        VkFence &getInFlightFence();
        void submitToCommandBuffer(VkCommandBuffer &commandBuffer);
    protected:
    private:
        std::unique_ptr<Device> &_device;
        VkSemaphore _imageAvailableSemaphore;
        VkSemaphore _renderFinishedSemaphore;
        VkFence _inFlightFence;
};

#endif /* !SYNCOBJ_HPP_ */
