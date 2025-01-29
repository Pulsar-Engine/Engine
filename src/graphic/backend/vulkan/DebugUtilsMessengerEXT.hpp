#ifndef DEBUGUTILSMESSENGEREXT_HPP_
    #define DEBUGUTILSMESSENGEREXT_HPP_

#include "Primitive.hpp"

class DebugUtilsMessengerEXT : public Primitive<VkDebugUtilsMessengerEXT> {
    public:
        DebugUtilsMessengerEXT();
        void setup(VkInstance *instance);
        ~DebugUtilsMessengerEXT();
        VkDebugUtilsMessengerCreateInfoEXT *getCreateInfo();
        DebugUtilsMessengerEXT &operator=(const DebugUtilsMessengerEXT &rvalue);
    protected:
    private:
        VkInstance *_instance;
        VkDebugUtilsMessengerCreateInfoEXT _createInfo;
};

#endif
