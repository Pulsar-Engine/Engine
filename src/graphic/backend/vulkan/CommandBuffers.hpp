#ifndef COMMANDBUFFERS_HPP_
    #define COMMANDBUFFERS_HPP_

    #include "CommandPool.hpp"
    #include "render/GraphicsPipeline.hpp"

class SyncObj;

class Instance;
class Image;

class CommandBuffers {
    public:
        CommandBuffers(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool);
        ~CommandBuffers();
        void record(Instance &instance, uint32_t imageIndex, uint32_t currentFrame);
        std::vector<VkCommandBuffer> &getCommandBuffers();
        std::vector<SyncObj> &getSyncObjs();
        static VkCommandBuffer beginSingleTimeCommands(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool);
        static void endSingleTimeCommands(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool, VkCommandBuffer commandBuffer);
        void transitionImageLayout(Image &image, VkImageLayout oldLayout, VkImageLayout newLayout);
        protected:
    private:
        std::unique_ptr<Device> &_device;
        std::unique_ptr<CommandPool> &_commandPool;
        alignas(VkCommandBuffer) std::vector<VkCommandBuffer> _commandBuffers;
        std::vector<SyncObj> _syncObjs;

};

#endif
