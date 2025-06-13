#ifndef TRANSFORM_COMPONENT_HPP_
#define TRANSFORM_COMPONENT_HPP_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
    glm::vec3 position {0.0f};
    glm::quat rotation {};
    glm::vec3 scale {1.0f};
};

#endif
