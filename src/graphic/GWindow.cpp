#include "GWindow.hpp"
#include "backend/vulkan/Instance.hpp"
#include "ECS/Core/Coordinator.hpp"

GWindow *GWindow::_instance = nullptr;

GWindow::GWindow(const int width, const int height, const char *title, bool fromEditor) : _width(width), _height(height), _title(title), _currentFrame(0), _camera(width, height) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    if (fromEditor) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    _primitive = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwSetInputMode(_primitive, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    _framebufferResized = false;
    glfwSetWindowUserPointer(_primitive, this);
    glfwSetFramebufferSizeCallback(_primitive, [](GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<GWindow*>(glfwGetWindowUserPointer(window));
        app->callbackResize(window, width, height);
    });
    if (_instance == nullptr)
        _instance = this;
    else
        throw std::runtime_error("Window instance already exists!");
    glfwSetCursorPosCallback(_primitive, [](GLFWwindow* window, double xpos, double ypos) {
        auto app = reinterpret_cast<GWindow*>(glfwGetWindowUserPointer(window));
        app->onMouseMove(xpos, ypos);
    });
}

void GWindow::onMouseMove(double xpos, double ypos) {
    _camera.changeDirection(xpos, ypos);
}

void GWindow::callbackResize(GLFWwindow *window, int width, int height) {
    (void) window;
    (void) width;
    (void) height;
    _framebufferResized = true;
}

void GWindow::loop(Instance &instance, Coordinator &coordinator) {
    _closed = false;
    _paused = false;
    double lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(_primitive) && !_closed) {
        instance.getDevice()->waitIdle();
        if (_paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastFrameTime);
        lastFrameTime = currentTime;
        if (glfwGetKey(_primitive, GLFW_KEY_W) == GLFW_PRESS)
            _camera.move(Direction::FORWARD, deltaTime);
        if (glfwGetKey(_primitive, GLFW_KEY_S) == GLFW_PRESS)
            _camera.move(Direction::BACKWARD, deltaTime);
        if (glfwGetKey(_primitive, GLFW_KEY_A) == GLFW_PRESS)
            _camera.move(Direction::LEFT, deltaTime);
        if (glfwGetKey(_primitive, GLFW_KEY_D) == GLFW_PRESS)
            _camera.move(Direction::RIGHT, deltaTime);
        if (glfwGetKey(_primitive, GLFW_KEY_SPACE) == GLFW_PRESS)
            _camera.move(Direction::UP, deltaTime);
        if (glfwGetKey(_primitive, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            _camera.move(Direction::DOWN, deltaTime);
        glfwPollEvents();
        coordinator.Update(deltaTime);
        drawFrame(instance);
    }
}

void GWindow::drawFrame(Instance &instance) {
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
    
    glm::mat4 view = glm::lookAt(_camera.getPos(), _camera.getPos() + _camera.getFront(), _camera.getUp());
    glm::mat4 proj = glm::perspective(
        glm::radians(_camera.getFOV()),
        static_cast<float>(_width) / static_cast<float>(_height),
        0.1f,
        100.0f
    );
    proj[1][1] *= -1;
    
    instance.getMeshManager().updateUniformBuffers(_currentFrame, view, proj);
    
    instance.getCommandBuffers()->record(instance, imageIndex, _currentFrame);
    
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

GWindow::~GWindow() {
    if (_primitive == nullptr)
        return;
    glfwDestroyWindow(_primitive);
    glfwTerminate();
}

GWindow *GWindow::getInstance() {
    return _instance;
}

void GWindow::close() {
    _closed = true;
}

void GWindow::togglePause() {
    _paused = !_paused;
}

std::atomic<bool> &GWindow::isClosed() {
    return _closed;
}

void GWindow::toggleShow() {
    if (glfwGetWindowAttrib(_primitive, GLFW_VISIBLE))
        glfwHideWindow(_primitive);
    else
        glfwShowWindow(_primitive);
}