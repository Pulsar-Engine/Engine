#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <chrono>

#include "graphic/backend/vulkan/Instance.hpp"

bool simpleWindowVulkan(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::chrono::seconds::period> deltaTime = currentTime - lastTime;
        if (deltaTime.count() > 5.0f)
            break;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return (true);
}

bool my_imp()
{
    Instance instance("Vulkan window");
    return (true);
}

constexpr bool (*tests[])(void) = {
    simpleWindowVulkan,
    my_imp,
};

int main(int argc, char **argv)
{
    int test = -1;
    if (argc == 2)
        test = std::stoi(argv[1]);
    for (int i = 0; tests[i] != nullptr; i++) {
        if (test == -1)
            tests[i]();
        else if (i == test)
            return (tests[i]() ? 0 : 1);
    }
    return (0);
}