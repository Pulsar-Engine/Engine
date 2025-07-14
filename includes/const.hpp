#include <unordered_map>
#include <memory>
#include <functional>

#include "components/HealthComponent.hpp"
#include "components/PositionComponent.hpp"
#include "components/UUIDComponent.hpp"
#include "components/VelocityComponent.hpp"

#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

static std::unordered_map<ComponentType, std::function<std::shared_ptr<IComponent>(void)>> component_list = {
    { HEALTH, [](void) { return std::make_shared<HealthComponent>(); } },
    { POSITION, [](void) { return std::make_shared<PositionComponent>(); } },
    { VELOCITY, [](void) { return std::make_shared<VelocityComponent>(); }},
    { UUID, [](void) { return std::make_shared<UUIDComponent>(); }},
};

#endif