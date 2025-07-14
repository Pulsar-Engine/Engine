#include "Coordinator.hpp"

void Coordinator::Init() {
    _entityManager = std::make_unique<EntityManager>();
    _componentManager = std::make_unique<ComponentManager>();
    _systemManager = std::make_unique<SystemManager>();
}

Entity Coordinator::CreateEntity() {
    return _entityManager->CreateEntity();
}

void Coordinator::DestroyEntity(Entity entity) {
    _entityManager->DestroyEntity(entity);
    _componentManager->entityDestroyed(entity);
    _systemManager->entityDestroyed(entity);
}
