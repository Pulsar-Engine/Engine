#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <chrono>
#include <thread>
#include <atomic>

#include "Engine.hpp"

#include "ECS/Core/Coordinator.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/VelocityComponent.hpp"
#include "ECS/Components/MeshComponent.hpp"
#include "ECS/Components/ColliderComponent.hpp"
#include "ECS/Components/GravityComponent.hpp"
#include "ECS/Systems/MovementSystem.hpp"
#include "ECS/Systems/CollisionSystem.hpp"
#include <iostream>

bool test_ecs()
{
    std::condition_variable cv;
    std::atomic<bool> ecsTestCompleted{false};
    std::thread render([&]() {
        Engine::initWindow(false);
        cv.notify_one();
        
        // Attendre que le test ECS soit complètement terminé
        while (!ecsTestCompleted.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        Engine::render();
        Engine::destroyWindow();
    });
    std::mutex mutex;
    std::unique_lock lock(mutex);
    cv.wait(lock, [] { return Engine::getInstance(); });
    
    Coordinator *coordinator = Engine::getCoordinator();
    bool testResult = false;
    
    try {
        coordinator->RegisterComponent<MeshComponent>();
        coordinator->RegisterComponent<TransformComponent>();
        coordinator->RegisterComponent<Velocity>();

        auto movementSystem = coordinator->RegisterSystem<MovementSystem>();
        Signature sig;
        sig.set(coordinator->GetComponentType<TransformComponent>());
        sig.set(coordinator->GetComponentType<Velocity>());
        sig.set(coordinator->GetComponentType<MeshComponent>());
        coordinator->SetSystemSignature<MovementSystem>(sig);

        // entité mobile
        Entity e = coordinator->CreateEntity();
        coordinator->AddComponent<TransformComponent>(e, {
            .position = glm::vec3(0.f),
            .rotation = glm::quat(),
            .scale = glm::vec3(1.f)
        });
        coordinator->AddComponent<Velocity>(e, {
            .linearVelocity = glm::vec3(2.0f, 0.0f, 0.0f)
        });
        coordinator->AddComponent<MeshComponent>(e, MeshComponent(Engine::getInstance()->addMesh("models/viking_room.obj", "textures/viking_room.png")));
        
        testResult = true;
        
    } catch (const std::exception& e) {
        std::cout << "Erreur dans test_ecs: " << e.what() << std::endl;
        testResult = false;
    }
    
    // Signaler que le test ECS est terminé AVANT de joindre le thread
    ecsTestCompleted.store(true);
    render.join();
    return testResult;
}

bool test_collision()
{
    std::cout << "=== Test Simple de Collision ===" << std::endl;
    
    std::condition_variable cv;
    std::atomic<bool> ecsTestCompleted{false};
    std::atomic<bool> engineReady{false};
    
    std::thread render([&]() {
        Engine::initWindow(false);
        engineReady.store(true);
        cv.notify_one();
        
        // Attendre que le test ECS soit complètement terminé
        while (!ecsTestCompleted.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        Engine::render();
        Engine::destroyWindow();
    });
    
    std::mutex mutex;
    std::unique_lock lock(mutex);
    cv.wait(lock, [&] { return engineReady.load(); });
    
    // Attendre un peu pour être sûr que l'initialisation est complète
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    Coordinator *coordinator = Engine::getCoordinator();
    bool testResult = false;
    
    // Vérifier que le coordinateur est valide
    if (!coordinator) {
        std::cout << "Erreur: Coordinator est null !" << std::endl;
        ecsTestCompleted.store(true);
        render.join();
        return false;
    }
    
    try {
        std::cout << "Enregistrement des composants..." << std::endl;
        
        // Enregistrer seulement les composants nécessaires
        coordinator->RegisterComponent<TransformComponent>();
        std::cout << "TransformComponent enregistré" << std::endl;
        
        coordinator->RegisterComponent<ColliderComponent>();
        std::cout << "ColliderComponent enregistré" << std::endl;
        
        std::cout << "Enregistrement du système..." << std::endl;
        
        // Enregistrer le système de collision
        auto collisionSystem = coordinator->RegisterSystem<CollisionSystem>();
        std::cout << "CollisionSystem enregistré" << std::endl;
        
        // Configurer la signature minimale
        Signature collisionSig;
        collisionSig.set(coordinator->GetComponentType<ColliderComponent>());
        collisionSig.set(coordinator->GetComponentType<TransformComponent>());
        coordinator->SetSystemSignature<CollisionSystem>(collisionSig);
        std::cout << "Signature configurée" << std::endl;
        
        std::cout << "Enregistrement du callback..." << std::endl;
        
        // Callback simple
        collisionSystem->RegisterCollisionCallback([](const CollisionInfo& collision) {
            std::cout << "Collision détectée entre " << collision.entityA << " et " << collision.entityB << std::endl;
        });
        std::cout << "Callback enregistré" << std::endl;
        
        std::cout << "Création des entités..." << std::endl;
        
        // Créer deux objets qui se chevauchent
        Entity boxA = coordinator->CreateEntity();
        std::cout << "Entité boxA créée: " << boxA << std::endl;
        
        coordinator->AddComponent<TransformComponent>(boxA, {
            .position = glm::vec3(0.0f, 0.0f, 0.0f),
            .rotation = glm::quat(),
            .scale = glm::vec3(1.0f)
        });
        std::cout << "TransformComponent ajouté à boxA" << std::endl;
        
        auto colliderA = ColliderComponent::CreateAABB(
            glm::vec3(-1.0f, -1.0f, -1.0f), 
            glm::vec3(1.0f, 1.0f, 1.0f)
        );
        std::cout << "Collider AABB créé pour boxA" << std::endl;
        
        coordinator->AddComponent<ColliderComponent>(boxA, std::move(colliderA));
        std::cout << "ColliderComponent ajouté à boxA" << std::endl;
        
        Entity boxB = coordinator->CreateEntity();
        std::cout << "Entité boxB créée: " << boxB << std::endl;
        
        coordinator->AddComponent<TransformComponent>(boxB, {
            .position = glm::vec3(0.5f, 0.0f, 0.0f),  // Position qui chevauche avec boxA
            .rotation = glm::quat(),
            .scale = glm::vec3(1.0f)
        });
        std::cout << "TransformComponent ajouté à boxB" << std::endl;
        
        auto colliderB = ColliderComponent::CreateAABB(
            glm::vec3(-1.0f, -1.0f, -1.0f), 
            glm::vec3(1.0f, 1.0f, 1.0f)
        );
        std::cout << "Collider AABB créé pour boxB" << std::endl;
        
        coordinator->AddComponent<ColliderComponent>(boxB, std::move(colliderB));
        std::cout << "ColliderComponent ajouté à boxB" << std::endl;
        
        std::cout << "Vérification des composants..." << std::endl;
        
        // Vérifier que les composants ont bien été ajoutés
        if (!coordinator->HasComponent<TransformComponent>(boxA)) {
            throw std::runtime_error("BoxA n'a pas de TransformComponent");
        }
        if (!coordinator->HasComponent<ColliderComponent>(boxA)) {
            throw std::runtime_error("BoxA n'a pas de ColliderComponent");
        }
        if (!coordinator->HasComponent<TransformComponent>(boxB)) {
            throw std::runtime_error("BoxB n'a pas de TransformComponent");
        }
        if (!coordinator->HasComponent<ColliderComponent>(boxB)) {
            throw std::runtime_error("BoxB n'a pas de ColliderComponent");
        }
        
        std::cout << "Tous les composants sont présents" << std::endl;
        
        std::cout << "Test d'une frame de collision..." << std::endl;
        
        // Petite pause pour s'assurer que tout est bien initialisé
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        try {
            std::cout << "Appel de coordinator->Update..." << std::endl;
            coordinator->Update(0.016f);
            std::cout << "coordinator->Update terminé avec succès" << std::endl;
        } catch (const std::exception& updateEx) {
            std::cout << "Erreur dans coordinator->Update: " << updateEx.what() << std::endl;
            throw;
        } catch (...) {
            std::cout << "Erreur inconnue dans coordinator->Update" << std::endl;
            throw;
        }
        
        std::cout << "Update terminé" << std::endl;
        
        std::cout << "Test simple terminé avec succès !" << std::endl;
        testResult = true;
        
    } catch (const std::exception& e) {
        std::cout << "Erreur dans le test: " << e.what() << std::endl;
        testResult = false;
    } catch (...) {
        std::cout << "Erreur inconnue dans le test de collision" << std::endl;
        testResult = false;
    }
    
    // Signaler que le test ECS est terminé AVANT de joindre le thread
    ecsTestCompleted.store(true);
    render.join();
    return testResult;
}

bool test_collision_simple()
{
    std::cout << "=== Test Simple de Configuration de Collision ===" << std::endl;
    
    std::condition_variable cv;
    std::atomic<bool> ecsTestCompleted{false};
    std::thread render([&]() {
        Engine::initWindow(false);
        cv.notify_one();
        
        // Attendre que le test ECS soit complètement terminé
        while (!ecsTestCompleted.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        Engine::render();
        Engine::destroyWindow();
    });
    std::mutex mutex;
    std::unique_lock lock(mutex);
    cv.wait(lock, [] { return Engine::getInstance(); });
    
    Coordinator *coordinator = Engine::getCoordinator();
    bool testResult = false;
    
    try {
        std::cout << "Enregistrement des composants..." << std::endl;
        
        coordinator->RegisterComponent<TransformComponent>();
        coordinator->RegisterComponent<ColliderComponent>();
        
        std::cout << "Enregistrement du système..." << std::endl;
        
        auto collisionSystem = coordinator->RegisterSystem<CollisionSystem>();
        
        Signature collisionSig;
        collisionSig.set(coordinator->GetComponentType<ColliderComponent>());
        collisionSig.set(coordinator->GetComponentType<TransformComponent>());
        coordinator->SetSystemSignature<CollisionSystem>(collisionSig);
        
        std::cout << "Création d'une entité simple..." << std::endl;
        
        Entity boxA = coordinator->CreateEntity();
        coordinator->AddComponent<TransformComponent>(boxA, {
            .position = glm::vec3(0.0f, 0.0f, 0.0f),
            .rotation = glm::quat(),
            .scale = glm::vec3(1.0f)
        });
        
        auto colliderA = ColliderComponent::CreateAABB(
            glm::vec3(-1.0f, -1.0f, -1.0f), 
            glm::vec3(1.0f, 1.0f, 1.0f)
        );
        coordinator->AddComponent<ColliderComponent>(boxA, std::move(colliderA));
        
        std::cout << "Entité créée avec succès. Test de configuration réussi !" << std::endl;
        testResult = true;
        
    } catch (const std::exception& e) {
        std::cout << "Erreur dans le test simple: " << e.what() << std::endl;
        testResult = false;
    } catch (...) {
        std::cout << "Erreur inconnue dans le test simple" << std::endl;
        testResult = false;
    }
    
    ecsTestCompleted.store(true);
    render.join();
    return testResult;
}

bool test_movement_gravity()
{
    std::cout << "=== Test Mouvement et Gravité ===" << std::endl;
    
    std::condition_variable cv;
    std::atomic<bool> ecsTestCompleted{false};
    std::thread render([&]() {
        Engine::initWindow(false);
        cv.notify_one();
        
        // Attendre que le test ECS soit complètement terminé
        while (!ecsTestCompleted.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        Engine::render();
        Engine::destroyWindow();
    });
    std::mutex mutex;
    std::unique_lock lock(mutex);
    cv.wait(lock, [] { return Engine::getInstance(); });
    
    Coordinator *coordinator = Engine::getCoordinator();
    bool testResult = false;
    
    try {
        // Enregistrer les composants
        coordinator->RegisterComponent<TransformComponent>();
        coordinator->RegisterComponent<Velocity>();
        coordinator->RegisterComponent<GravityComponent>();
        coordinator->RegisterComponent<MeshComponent>();
        
        // Enregistrer le système de mouvement
        auto movementSystem = coordinator->RegisterSystem<MovementSystem>();
        
        // Configurer la signature pour le mouvement
        Signature movementSig;
        movementSig.set(coordinator->GetComponentType<TransformComponent>());
        movementSig.set(coordinator->GetComponentType<Velocity>());
        coordinator->SetSystemSignature<MovementSystem>(movementSig);
        
        // Créer une entité avec mouvement et gravité
        Entity entity = coordinator->CreateEntity();
        coordinator->AddComponent<TransformComponent>(entity, {
            .position = glm::vec3(0.0f, 10.0f, 0.0f),
            .rotation = glm::quat(),
            .scale = glm::vec3(1.0f)
        });
        coordinator->AddComponent<Velocity>(entity, {
            .linearVelocity = glm::vec3(2.0f, 0.0f, 0.0f)
        });
        GravityComponent gravityComp;
        gravityComp.gravity = 9.81;  // Gravité positive vers le bas
        gravityComp.speed = 0.0;
        gravityComp.maxSpeed = 100.0;
        coordinator->AddComponent<GravityComponent>(entity, std::move(gravityComp));
        coordinator->AddComponent<MeshComponent>(entity, MeshComponent(
            Engine::getInstance()->addMesh("models/viking_room.obj", "textures/viking_room.png")
        ));
        
        auto& transform = coordinator->GetComponent<TransformComponent>(entity);
        auto& velocity = coordinator->GetComponent<Velocity>(entity);
        
        std::cout << "Position initiale: (" << transform.position.x << ", " 
                  << transform.position.y << ", " << transform.position.z << ")" << std::endl;
        std::cout << "Vélocité initiale: (" << velocity.linearVelocity.x << ", " 
                  << velocity.linearVelocity.y << ", " << velocity.linearVelocity.z << ")" << std::endl;
        
        // Simuler quelques frames
        for (int i = 0; i < 5; ++i) {
            coordinator->Update(0.016f);  // 60 FPS
            std::cout << "Frame " << (i+1) << " - Position: (" 
                      << transform.position.x << ", " << transform.position.y << ", " 
                      << transform.position.z << "), Vélocité: (" 
                      << velocity.linearVelocity.x << ", " << velocity.linearVelocity.y 
                      << ", " << velocity.linearVelocity.z << ")" << std::endl;
        }
        
        std::cout << "Test mouvement et gravité terminé avec succès !" << std::endl;
        testResult = true;
        
    } catch (const std::exception& e) {
        std::cout << "Erreur: " << e.what() << std::endl;
        testResult = false;
    }
    
    // Signaler que le test ECS est terminé AVANT de joindre le thread
    ecsTestCompleted.store(true);
    render.join();
    return testResult;
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
    Engine::getInstance()->addMesh("models/viking_room.obj", "textures/viking_room.png");
    Engine::render();
    Engine::destroyWindow();
    return (true);
}

constexpr bool (*tests[])(void) = {
    simpleWindowVulkan,
    my_imp,
    test_ecs,
    test_collision,
    test_movement_gravity,
    test_collision_simple,
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