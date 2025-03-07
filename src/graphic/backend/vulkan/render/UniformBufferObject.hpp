#include <glm/glm.hpp>

#ifndef UNIFORMBUFFEROBJECT_HPP_
#define UNIFORMBUFFEROBJECT_HPP_

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

#endif
