#ifndef COORDINATOR_HPP_
#define COORDINATOR_HPP_

#include <memory>
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Types.hpp"

class Coordinator {
public:
    void Init();

    Entity CreateEntity();

    void DestroyEntity(Entity entity);

    template<typename T>
    void RegisterComponent() {
        _componentManager->registerComponent<T>();
    }

    template<typename T>
    void AddComponent(Entity entity, T component) {
        _componentManager->addComponent<T>(entity, component);
        auto signature = _entityManager->GetSignature(entity);
        signature.set(_componentManager->getComponentType<T>(), true);
        _entityManager->SetSignature(entity, signature);
        _systemManager->entitySignatureChanged(entity, signature);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        _componentManager->removeComponent<T>(entity);
        auto signature = _entityManager->GetSignature(entity);
        signature.set(_componentManager->getComponentType<T>(), false);
        _entityManager->SetSignature(entity, signature);
        _systemManager->entitySignatureChanged(entity, signature);
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        return _componentManager->getComponent<T>(entity);
    }

    template<typename T>
    ComponentType GetComponentType() {
        return _componentManager->getComponentType<T>();
    }

    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        return _systemManager->registerSystem<T>();
    }

    template<typename T>
    void SetSystemSignature(Signature signature) {
        _systemManager->setSignature<T>(signature);
    }

private:
    std::unique_ptr<ComponentManager> _componentManager;
    std::unique_ptr<EntityManager> _entityManager;
    std::unique_ptr<SystemManager> _systemManager;
};

#endif
