#include "CommandBuffers.hpp"
#include "Instance.hpp"
#include "render/Image.hpp"
#include "render/DepthResources.hpp"

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
    vkFreeCommandBuffers(_device->getPrimitive(), _commandPool->getPrimitive(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
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

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkExtent2D swapChainExtent = instance.getSwapchain()->getExtent();
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = instance.getGraphicsPipeline()->getRenderPass()->getPrimitive();
    renderPassInfo.framebuffer = instance.getFrameBuffers()->getSwapChainFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
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
    vkCmdBindIndexBuffer(command, instance.getIndexBuffer()->getPrimitive(), 0, VK_INDEX_TYPE_UINT32);
    
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

VkCommandBuffer CommandBuffers::beginSingleTimeCommands(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool->getPrimitive();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device->getPrimitive(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void CommandBuffers::endSingleTimeCommands(std::unique_ptr<Device> &device, std::unique_ptr<CommandPool> &commandPool, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device->getPresentQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->getPresentQueue());
    vkFreeCommandBuffers(device->getPrimitive(), commandPool->getPrimitive(), 1, &commandBuffer);
}

void CommandBuffers::transitionImageLayout(Image &image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(_device, _commandPool);
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.getPrimitive();

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    
        if (DepthResources::hasStencilComponent(image.getFormat())) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }
    

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
    endSingleTimeCommands(_device, _commandPool, commandBuffer);
}