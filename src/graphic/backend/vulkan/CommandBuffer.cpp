#include "CommandBuffer.hpp"

CommandBuffer::CommandBuffer(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool) : _device(device), _commandPool(commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool->getPrimitive();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    
    if (vkAllocateCommandBuffers(device->getPrimitive(), &allocInfo, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");
}

CommandBuffer::~CommandBuffer()
{
    if (_primitive == VK_NULL_HANDLE)
        return;
    vkFreeCommandBuffers(_device->getPrimitive(), _commandPool->getPrimitive(), 1, &_primitive);
}

void CommandBuffer::record(uint32_t imageIndex, std::vector<VkFramebuffer> &swapChainFramebuffers, std::unique_ptr<GraphicsPipeline> &graphicsPipeline, VkExtent2D &swapChainExtent)
{
    vkResetCommandBuffer(_primitive, 0);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(_primitive, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = graphicsPipeline->getRenderPass()->getPrimitive();
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(_primitive, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(_primitive, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPrimitive());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(_primitive, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(_primitive, 0, 1, &scissor);
    vkCmdDraw(_primitive, 3, 1, 0, 0);
    vkCmdEndRenderPass(_primitive);

    if (vkEndCommandBuffer(_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to record command buffer!");
}