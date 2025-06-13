#include "ComponentManager.hpp"
#include <stdexcept>

void ComponentManager::entityDestroyed(Entity entity) {
    for (auto const& pair : _componentArrays) {
        auto const& component = pair.second;
        component->entityDestroyed(entity);
    }
}
