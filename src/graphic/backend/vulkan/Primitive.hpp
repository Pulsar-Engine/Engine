#ifndef PRIMITIVE_HPP_
    #define PRIMITIVE_HPP_

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include <memory>

const int MAX_FRAMES_IN_FLIGHT = 2;

template<typename T>
class Primitive {
    public:
        Primitive() : _primitive(VK_NULL_HANDLE) {}
        virtual ~Primitive() = default;
        T getPrimitive() const { return _primitive; }
    protected:
        T _primitive;
    private:
};

#endif
