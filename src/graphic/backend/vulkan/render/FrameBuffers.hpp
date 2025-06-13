#ifndef FRAMEBUFFERS_HPP_
    #define FRAMEBUFFER_HPP_

    #include "../Primitive.hpp"
    #include "../ImageView.hpp"
    #include "GraphicsPipeline.hpp"
    #include "DepthResources.hpp"

    #include <vector>

class FrameBuffers {
    public:
        FrameBuffers(std::unique_ptr<GraphicsPipeline> &graphicsPipeline, DepthResources &_depthResources, std::unique_ptr<Device> &device,std::vector<ImageView> &imageViews, VkExtent2D &extent);
        ~FrameBuffers();
        std::vector<VkFramebuffer> &getSwapChainFramebuffers();
    protected:
    private:
        std::vector<VkFramebuffer> _swapChainFramebuffers;
        std::unique_ptr<Device> &_device;
};

#endif
