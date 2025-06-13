#include "DebugUtilsMessengerEXT.hpp"
#include "Instance.hpp"


DebugUtilsMessengerEXT::DebugUtilsMessengerEXT() : _instance(nullptr),  _createInfo({})
{

    _createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    _createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    _createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    _createInfo.pfnUserCallback = Instance::debugCallback;
    _createInfo.pUserData = nullptr;
}

void DebugUtilsMessengerEXT::setup(VkInstance *instance)
{
    _instance = instance;
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(*_instance, "vkCreateDebugUtilsMessengerEXT");
    if (func == nullptr || func(*_instance, &_createInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug messenger!");
}

VkDebugUtilsMessengerCreateInfoEXT *DebugUtilsMessengerEXT::getCreateInfo()
{
    return &_createInfo;
}

DebugUtilsMessengerEXT::~DebugUtilsMessengerEXT()
{
    if (_instance == nullptr)
        return;
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(*_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(*_instance, _primitive, nullptr);
}

DebugUtilsMessengerEXT &DebugUtilsMessengerEXT::operator=(const DebugUtilsMessengerEXT &rvalue)
{
    _primitive = rvalue._primitive;
    return *this;
}
