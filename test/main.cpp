#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <chrono>

#include "Engine.hpp"

#include "../ECS/Core/Coordinator.hpp"
#include "../ECS/Components/TransformComponent.hpp"
#include "../ECS/Components/VelocityComponent.hpp"
#include "../ECS/Systems/MovementSystem.hpp"
#include <iostream>

bool test_ecs()
{
    Coordinator coordinator;
    coordinator.Init();

    coordinator.RegisterComponent<Transform>();
    coordinator.RegisterComponent<Velocity>();

    auto movementSystem = coordinator.RegisterSystem<MovementSystem>();
    Signature sig;
    sig.set(coordinator.GetComponentType<Transform>());
    sig.set(coordinator.GetComponentType<Velocity>());
    coordinator.SetSystemSignature<MovementSystem>(sig);

    // entité mobile
    Entity e = coordinator.CreateEntity();
    coordinator.AddComponent<Transform>(e, {
        .position = glm::vec3(0.f),
        .rotation = glm::quat(),
        .scale = glm::vec3(1.f)
    });
    coordinator.AddComponent<Velocity>(e, {
        .linearVelocity = glm::vec3(2.0f, 0.0f, 0.0f)
    });

    // simulation 5 frames
    for (int i = 0; i < 5; ++i) {
        movementSystem->Update(1.0f, coordinator);
        auto& t = coordinator.GetComponent<Transform>(e);
        std::cout << "[Frame " << i << "] Position: "
                  << t.position.x << ", " << t.position.y << ", " << t.position.z << "\n";
    }

    return true;
}

bool test_scene_manager()
{
    Coordinator coordinator;
    coordinator.Init();

    SceneManager sceneManager(coordinator);

    sceneManager.createScene("MainMenu");
    sceneManager.createScene("Gameplay");

    sceneManager.switchToScene("MainMenu");
    std::cout << "Active Scene: " << sceneManager.getCurrentScene()->name << "\n";

    sceneManager.switchToScene("Gameplay");
    std::cout << "Active Scene: " << sceneManager.getCurrentScene()->name << "\n";

    sceneManager.removeScene("MainMenu");

    if (sceneManager.getScene("MainMenu") == nullptr)
        std::cout << "MainMenu scene successfully removed.\n";
    else
        std::cout << "MainMenu scene still exists!\n";

    return true;
}

bool simpleWindowVulkan(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
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
    Engine::initWindow(false);
    Engine::render();
    Engine::destroyWindow();
    return (true);
}

constexpr bool (*tests[])(void) = {
    simpleWindowVulkan,
    my_imp,
    test_ecs,
    test_scene_manager,
    nullptr};

int main(int argc, char **argv)
{
    int test = -1;
    if (argc == 2)
    {
        try
        {
            test = std::stoi(argv[1]);
        }
        catch (const std::exception &)
        {
        }
    }
    for (int i = 0; tests[i] != nullptr; i++)
    {
        if (test == -1)
            tests[i]();
        else if (i == test)
            return (tests[i]() ? 0 : 1);
    }
    return (0);
}