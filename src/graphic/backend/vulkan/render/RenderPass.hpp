
#ifndef RENDERPASS_HPP_
    #define RENDERPASS_HPP_

    #include "../Primitive.hpp"
    #include "../Device.hpp"

class RenderPass : public Primitive<VkRenderPass> {
    public:
        RenderPass(std::unique_ptr<Device> &device, VkFormat &format);
        ~RenderPass();
    protected:
    private:
        std::unique_ptr<Device> &_device;
};

#endif
