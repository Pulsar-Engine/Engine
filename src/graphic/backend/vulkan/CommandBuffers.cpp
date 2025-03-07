#include "CommandBuffers.hpp"
#include "Instance.hpp"

CommandBuffers::CommandBuffers(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool) : _device(device), _commandPool(commandPool)
{
    _commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _syncObjs.reserve(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool->getPrimitive();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    
    if (vkAllocateCommandBuffers(device->getPrimitive(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");
    

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        _syncObjs.emplace_back(device);
}

CommandBuffers::~CommandBuffers()
{
    _syncObjs.clear();
    vkFreeCommandBuffers(_device->getPrimitive(), _commandPool->getPrimitive(), 1, _commandBuffers.data());
}

void CommandBuffers::record(Instance &instance, uint32_t imageIndex, uint32_t currentFrame)
{
    VkCommandBuffer &command = _commandBuffers[currentFrame];
    vkResetCommandBuffer(command, 0);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(command, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin recording command buffer!");

    VkExtent2D swapChainExtent = instance.getSwapchain()->getExtent();
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = instance.getGraphicsPipeline()->getRenderPass()->getPrimitive();
    renderPassInfo.framebuffer = instance.getFrameBuffers()->getSwapChainFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, instance.getGraphicsPipeline()->getPrimitive());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    VkBuffer vertexBuffers[] = { instance.getVertexBuffer()->getPrimitive() };
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(command, instance.getIndexBuffer()->getPrimitive(), 0, VK_INDEX_TYPE_UINT16);
    
    vkCmdSetScissor(command, 0, 1, &scissor);
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, instance.getGraphicsPipeline()->getPipelineLayout(), 0, 1, &instance.getDescriptorSets()->getPrimitive()[currentFrame], 0, nullptr);
    vkCmdDrawIndexed(command, static_cast<uint32_t>(instance.getIndices().size()), 1, 0, 0, 0);
    vkCmdEndRenderPass(command);

    if (vkEndCommandBuffer(command) != VK_SUCCESS)
        throw std::runtime_error("failed to record command buffer!");
}

std::vector<VkCommandBuffer> &CommandBuffers::getCommandBuffers()
{
    return _commandBuffers;
}

std::vector<SyncObj> &CommandBuffers::getSyncObjs()
{
    return _syncObjs;
}