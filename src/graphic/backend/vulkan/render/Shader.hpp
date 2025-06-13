
#ifndef SHADER_HPP_
    #define SHADER_HPP_

    #include "../Primitive.hpp"
    #include "../Device.hpp"

    #include <vector>
    #include <fstream>
    #include <cstring>

class Shader : public Primitive<VkShaderModule> {
    public:
        Shader(std::unique_ptr<Device> &device, const std::vector<char> code);
        Shader(std::unique_ptr<Device> &device, const std::string &filename);
        ~Shader();
        static std::vector<char> readFile(const std::string &filename);
    protected:
    private:
        std::vector<char> _code;
        std::unique_ptr<Device> &_device;
};

#endif
