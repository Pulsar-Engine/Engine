#ifndef VELOCITY_COMPONENT_HPP_
#define VELOCITY_COMPONENT_HPP_

#include <glm/vec3.hpp>

struct Velocity {
    glm::vec3 linearVelocity {0.0f};   // movement
    glm::vec3 angularVelocity {0.0f};  // rotation, unused for now
};

#endif
