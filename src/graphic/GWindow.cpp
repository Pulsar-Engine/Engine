
#include "GWindow.hpp"
#include "GWindow.hpp"
#include "backend/vulkan/Instance.hpp"

GWindow *GWindow::_instance = nullptr;

GWindow::GWindow(const int width, const int height, const char *title, bool fromEditor) : _width(width), _height(height), _title(title), _currentFrame(0) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    if (fromEditor) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    _cameraPos = glm::vec3(0.0f, -3.0f, 1.5f);
    _cameraFront = glm::vec3(0.0f, 1.0f, -0.5f);
    _cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);
    _yaw = glm::degrees(atan2(_cameraFront.y, _cameraFront.x));
    _pitch = glm::degrees(asin(_cameraFront.z));
    _fov = 45.0f;
    _firstMouse = true;
    _lastX = _width / 2.0;
    _lastY = _height / 2.0;
    _cursorDisabled = false;
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
    if (_firstMouse) {
        _lastX = xpos;
        _lastY = ypos;
        _firstMouse = false;
        return;
    }
    double xoffset = _lastX - xpos;
    double yoffset = _lastY - ypos;
    _lastX = xpos;
    _lastY = ypos;
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    _yaw   += static_cast<float>(xoffset);
    _pitch += static_cast<float>(yoffset);
    if (_pitch > 89.0f) _pitch = 89.0f;
    if (_pitch < -89.0f) _pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    direction.y = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    direction.z = sin(glm::radians(_pitch));
    _cameraFront = glm::normalize(direction);
}

void GWindow::callbackResize(GLFWwindow *window, int width, int height) {
    (void) window;
    (void) width;
    (void) height;
    _framebufferResized = true;
}

void GWindow::loop(Instance &instance) {
    _finished = false;
    float cameraSpeed = 2.5f;
    double lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(_primitive) && !_finished) {
        instance.getDevice()->waitIdle();
        if (_paused)
            continue;
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastFrameTime);
        lastFrameTime = currentTime;
        if (glfwGetKey(_primitive, GLFW_KEY_W) == GLFW_PRESS)
            _cameraPos += cameraSpeed * _cameraFront * deltaTime;
        if (glfwGetKey(_primitive, GLFW_KEY_S) == GLFW_PRESS)
            _cameraPos -= cameraSpeed * _cameraFront * deltaTime;
        if (glfwGetKey(_primitive, GLFW_KEY_A) == GLFW_PRESS)
            _cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed * deltaTime;
        if (glfwGetKey(_primitive, GLFW_KEY_D) == GLFW_PRESS)
            _cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed * deltaTime;
        if (glfwGetKey(_primitive, GLFW_KEY_SPACE) == GLFW_PRESS)
            _cameraPos += _cameraUp * cameraSpeed * deltaTime;
        if (glfwGetKey(_primitive, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            _cameraPos -= _cameraUp * cameraSpeed * deltaTime;
        glfwPollEvents();
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

void GWindow::updateUniformBuffer(Buffer &uniformBuffer) {
    UniformBufferObject ubo = {};
    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
    ubo.proj = glm::perspective(
        glm::radians(_fov),
        static_cast<float>(_width) / static_cast<float>(_height),
        0.1f,
        100.0f
    );
    ubo.proj[1][1] *= -1;
    uniformBuffer.copyData(&ubo);
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
    _finished = true;
}

void GWindow::togglePause() {
    _paused = !_paused;
}

void GWindow::toggleShow() {
    if (glfwGetWindowAttrib(_primitive, GLFW_VISIBLE))
        glfwHideWindow(_primitive);
    else
        glfwShowWindow(_primitive);
}