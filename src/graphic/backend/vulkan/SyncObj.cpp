#include "SyncObj.hpp"

SyncObj::SyncObj(std::unique_ptr<Device> &device) : _device(device)
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(_device->getPrimitive(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(_device->getPrimitive(), &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS)
        throw std::runtime_error("failed to create semaphores!");
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateFence(_device->getPrimitive(), &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS)
        throw std::runtime_error("failed to create fences!");
}

SyncObj::~SyncObj()
{
    vkDeviceWaitIdle(_device->getPrimitive());
    vkDestroySemaphore(_device->getPrimitive(), _imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(_device->getPrimitive(), _renderFinishedSemaphore, nullptr);
    vkDestroyFence(_device->getPrimitive(), _inFlightFence, nullptr);
}

VkSemaphore &SyncObj::getImageAvailableSemaphore()
{
    return _imageAvailableSemaphore;
}

VkSemaphore &SyncObj::getRenderFinishedSemaphore()
{
    return _renderFinishedSemaphore;
}

VkFence &SyncObj::getInFlightFence()
{
    return _inFlightFence;
}

void SyncObj::submitToCommandBuffer(VkCommandBuffer &commandBuffer)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;    
    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    if (vkQueueSubmit(_device->getGraphicsQueue(), 1, &submitInfo, _inFlightFence) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer!");
}
