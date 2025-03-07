#ifndef COMMANDBUFFERS_HPP_
    #define COMMANDBUFFERS_HPP_

    #include "CommandPool.hpp"
    #include "render/GraphicsPipeline.hpp"

class SyncObj;

class Instance;
class CommandBuffers {
    public:
        CommandBuffers(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool);
        ~CommandBuffers();
        void record(Instance &instance, uint32_t imageIndex, uint32_t currentFrame);
        std::vector<VkCommandBuffer> &getCommandBuffers();
        std::vector<SyncObj> &getSyncObjs();
    protected:
    private:
        std::unique_ptr<Device> &_device;
        std::unique_ptr<CommandPool> &_commandPool;
        alignas(VkCommandBuffer) std::vector<VkCommandBuffer> _commandBuffers;
        std::vector<SyncObj> _syncObjs;

};

#endif
