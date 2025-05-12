#include "Shader.hpp"
#include <iostream>
#include <string.h>

Shader::Shader(std::unique_ptr<Device> &device, const std::vector<char> code) : _code(code), _device(device)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    if (vkCreateShaderModule(_device->getPrimitive(), &createInfo, nullptr, &_primitive) != VK_SUCCESS)
        throw std::runtime_error("failed to create shader module!");
}

std::vector<char> Shader::readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        throw std::runtime_error("failed to open file! : " + filename);
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> code;
    code.resize(fileSize);g
    file.seekg(0);
    file.read(code.data(), fileSize);
    file.close();
    return code;
}

Shader::Shader(std::unique_ptr<Device> &device, const std::string &filename) : Shader(device, readFile(filename))
{

}

Shader::~Shader()
{
    if (_primitive == VK_NULL_HANDLE)
        return;
    vkDestroyShaderModule(_device->getPrimitive(), _primitive, nullptr);
}
