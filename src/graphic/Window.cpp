
#include "Window.hpp"
#include "backend/vulkan/Instance.hpp"

Window::Window(const int width, const int height, const char *title) : _width(width), _height(height), _title(title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _primitive = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void Window::loop(Instance &instance) {
    while (!glfwWindowShouldClose(_primitive)) {
        glfwPollEvents();
        drawFrame(instance);
    }
    instance.getDevice()->waitIdle();
}

void Window::drawFrame(Instance &instance) {
    uint32_t imageIndex;
    vkWaitForFences(instance.getDevice()->getPrimitive(), 1, &instance.getSyncObj()->getInFlightFence(), VK_TRUE, UINT64_MAX);
    vkResetFences(instance.getDevice()->getPrimitive(), 1, &instance.getSyncObj()->getInFlightFence());

    vkAcquireNextImageKHR(
        instance.getDevice()->getPrimitive(),
        instance.getSwapchain()->getPrimitive(),
        UINT64_MAX,
        instance.getSyncObj()->getImageAvailableSemaphore(),
        VK_NULL_HANDLE,
        &imageIndex
    );

    auto commandBufferPrimitive = instance.getCommandBuffer()->getPrimitive();
    instance.getCommandBuffer()->record(imageIndex, 
        instance.getFrameBuffers()->getSwapChainFramebuffers(),
        instance.getGraphicsPipeline(),
        instance.getSwapchain()->getExtent()
    );

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;

    VkSemaphore waitSemaphores[] = { instance.getSyncObj()->getImageAvailableSemaphore() };
    submitInfo.pWaitSemaphores = waitSemaphores;

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBufferPrimitive;

    VkSemaphore signalSemaphores[] = { instance.getSyncObj()->getRenderFinishedSemaphore() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(instance.getDevice()->getGraphicsQueue(), 1, &submitInfo, instance.getSyncObj()->getInFlightFence());

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { instance.getSwapchain()->getPrimitive() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(instance.getDevice()->getPresentQueue(), &presentInfo);
}

Window::~Window() {
    if (_primitive == nullptr)
        return;
    glfwDestroyWindow(_primitive);
    glfwTerminate();
}
