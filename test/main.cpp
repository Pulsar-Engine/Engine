#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

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
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return (true);
}

constexpr bool (*tests[])(void) = {
    simpleWindowVulkan,
};

int main(int argc, char **argv)
{
    if (argc != 2)
        return (84);
    int test = std::stoi(argv[1]);
    for (int i = 0; tests[i] != nullptr; i++) {
        if (i == test)
            return (tests[i]() ? 0 : 1);
    }
    return (0);
}