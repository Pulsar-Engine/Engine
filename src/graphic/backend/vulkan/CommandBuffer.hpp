#ifndef COMMANDBUFFER_HPP_
    #define COMMANDBUFFER_HPP_

    #include "Primitive.hpp"
    #include "CommandPool.hpp"
    #include "render/GraphicsPipeline.hpp"

class CommandBuffer : public Primitive<VkCommandBuffer> {
    public:
        CommandBuffer(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool);
        ~CommandBuffer();
        void record(uint32_t imageIndex, std::vector<VkFramebuffer> &swapChainFramebuffers, std::unique_ptr<GraphicsPipeline> &graphicsPipeline, VkExtent2D &swapChainExtent);
    protected:
    private:
        std::unique_ptr<Device> &_device;
        std::unique_ptr<CommandPool> &_commandPool;

};

#endif
