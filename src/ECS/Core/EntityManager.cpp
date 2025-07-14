#include "EntityManager.hpp"
#include "Types.hpp"
#include <stdexcept>

EntityManager::EntityManager() {
    for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
        _availableEntities.push(entity);
    }
}

Entity EntityManager::CreateEntity() {
    if (_livingEntityCount >= MAX_ENTITIES)
        throw std::runtime_error("Too many entities in existence.");

    Entity id = _availableEntities.front();
    _availableEntities.pop();
    ++_livingEntityCount;

    return id;
}

void EntityManager::DestroyEntity(Entity entity) {
    if (entity >= MAX_ENTITIES)
        throw std::runtime_error("Entity out of range.");

    _signatures[entity].reset();
    _availableEntities.push(entity);
    --_livingEntityCount;
}

void EntityManager::SetSignature(Entity entity, Signature signature) {
    if (entity >= MAX_ENTITIES)
        throw std::runtime_error("Entity out of range.");

    _signatures[entity] = signature;
}

Signature EntityManager::GetSignature(Entity entity) {
    if (entity >= MAX_ENTITIES)
        throw std::runtime_error("Entity out of range.");

    return _signatures[entity];
}
