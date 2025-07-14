#include "SystemManager.hpp"

void SystemManager::entityDestroyed(Entity entity) {
    for (auto const& pair : _systems) {
        auto const& system = pair.second;
        system->entities.erase(entity);
    }
}

void SystemManager::updateAllSystems(float dt, Coordinator &coordinator) {
    for (auto &pair : _systems)
        pair.second->Update(dt, coordinator);
}