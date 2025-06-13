#pragma once

#include "Types.hpp"
#include <queue>
#include <array>
#include <bitset>
#include <stdexcept>

class EntityManager {
public:
    EntityManager();

    Entity CreateEntity();
    void DestroyEntity(Entity entity);

    void SetSignature(Entity entity, Signature signature);
    Signature GetSignature(Entity entity);

private:
    std::queue<Entity> _availableEntities;
    std::array<Signature, MAX_ENTITIES> _signatures;
    uint32_t _livingEntityCount = 0;
};
