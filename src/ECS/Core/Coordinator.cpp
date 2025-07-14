#include "Coordinator.hpp"

Coordinator::Coordinator() {
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

void Coordinator::Update(float dt) {
    _systemManager->updateAllSystems(dt, *this);
}
