#ifndef BUFFER_HPP_
    #define BUFFER_HPP_

    #include "Primitive.hpp"
    #include "render/Vertex.hpp"
    #include "render/Image.hpp"

    #include <vector>

class Instance;
class Device;
class CommandPool;
class Image;

class Buffer : public Primitive<VkBuffer> {
    public:
        Buffer(Instance &instance, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Buffer();
        void bindImage(Instance &instance, VkImage &image, VkMemoryPropertyFlags properties);
        void map();
        void copyTo(std::unique_ptr<CommandPool> &commandPool, Buffer &dstBuffer);
        void copyToImage(std::unique_ptr<CommandPool> &commandPool, Image &image);
        void mapTo(void *data);
        VkDeviceSize getSize();
        void copyData(void *data);
        void CPUToGPU(Instance &instance, void *data);
    protected:
        std::unique_ptr<Device> &_device;
        VkDeviceMemory _memory;
        VkDeviceMemory _oldMemory;
        void *_data;
        VkDeviceSize _size;
    private:
};

#endif
