#ifndef UNIFORMBUFFEROBJECT_HPP_
    #define UNIFORMBUFFEROBJECT_HPP_

    #define GLM_FORCE_RADIANS
    #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
    #include <glm/glm.hpp>

struct UniformBufferObject {
    glm::mat4 model;
    alignas(16) glm::mat4 view;
    glm::mat4 proj;
};

#endif
