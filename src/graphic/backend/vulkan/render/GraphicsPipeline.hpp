#ifndef GRAPHICSPIPELINE_HPP_
    #define GRAPHICSPIPELINE_HPP_

    #include "../Primitive.hpp"
    #include "../Device.hpp"
    #include "../Swapchain.hpp"
    #include "Shader.hpp"
    #include "RenderPass.hpp"
    #include "../DescriptorSetLayout.hpp"

    #include <vector>

class GraphicsPipeline : public Primitive<VkPipeline> {
    public:
        GraphicsPipeline(std::unique_ptr<Device> &device, std::unique_ptr<DescriptorSetLayout> &descriptorSetLayout, std::unique_ptr<Swapchain> &swapchain);
        ~GraphicsPipeline();
        std::unique_ptr<RenderPass> &getRenderPass();
    protected:
    private:
        std::unique_ptr<Device> &_device;
        VkPipelineLayout pipelineLayout;
        std::unique_ptr<RenderPass> _renderPass;
};

#endif /* !GRAPHICSPIPELINE_HPP_ */
