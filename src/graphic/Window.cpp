
#include "Window.hpp"
#include "backend/vulkan/Instance.hpp"

Window::Window(const int width, const int height, const char *title) : _width(width), _height(height), _title(title), _currentFrame(0) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _primitive = glfwCreateWindow(width, height, title, nullptr, nullptr);
    _framebufferResized = false;
    glfwSetWindowUserPointer(_primitive, this);
    glfwSetFramebufferSizeCallback(_primitive, [](GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        app->callbackResize(window, width, height);
    });
}

void Window::callbackResize(GLFWwindow *window, int width, int height) {
    (void) window;
    (void) width;
    (void) height;
    _framebufferResized = true;
}

void Window::loop(Instance &instance) {
    while (!glfwWindowShouldClose(_primitive)) {
        glfwPollEvents();
        drawFrame(instance);
    }
    instance.getDevice()->waitIdle();
}

void Window::drawFrame(Instance &instance) {
    SyncObj &syncObj = instance.getCommandBuffers()->getSyncObjs()[_currentFrame];
    VkCommandBuffer &commandBuffer = instance.getCommandBuffers()->getCommandBuffers()[_currentFrame];

    vkWaitForFences(instance.getDevice()->getPrimitive(), 1, &syncObj.getInFlightFence(), VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        instance.getDevice()->getPrimitive(),
        instance.getSwapchain()->getPrimitive(),
        UINT64_MAX,
        syncObj.getImageAvailableSemaphore(),
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        instance.recreateSwapchain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(instance.getDevice()->getPrimitive(), 1, &syncObj.getInFlightFence());

    instance.getCommandBuffers()->record(instance, imageIndex, _currentFrame);
    updateUniformBuffer(instance.getUniformBuffers()[_currentFrame]);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;

    VkSemaphore waitSemaphores[] = { syncObj.getImageAvailableSemaphore() };
    submitInfo.pWaitSemaphores = waitSemaphores;

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = { syncObj.getRenderFinishedSemaphore() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(instance.getDevice()->getGraphicsQueue(), 1, &submitInfo, syncObj.getInFlightFence());

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { instance.getSwapchain()->getPrimitive() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(instance.getDevice()->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized) {
        _framebufferResized = false;
        instance.recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Window::updateUniformBuffer(Buffer &uniformBuffer) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo = {};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(
        glm::radians(45.0f),
        800.0f / 600.0f,
        0.1f,
        10.0f
    );
    ubo.proj[1][1] *= -1;
    uniformBuffer.copyData(&ubo);
}

Window::~Window() {
    if (_primitive == nullptr)
        return;
    glfwDestroyWindow(_primitive);
    glfwTerminate();
}
