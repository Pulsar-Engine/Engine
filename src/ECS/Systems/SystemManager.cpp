#include "SystemManager.hpp"

void SystemManager::entityDestroyed(Entity entity) {
    for (auto const& pair : _systems) {
        auto const& system = pair.second;
        system->entities.erase(entity);
    }
}
