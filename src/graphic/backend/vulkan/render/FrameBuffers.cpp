#include "FrameBuffers.hpp"

FrameBuffers::FrameBuffers(std::unique_ptr<GraphicsPipeline> &graphicsPipeline, DepthResources &depthResources, std::unique_ptr<Device> &device,std::vector<ImageView> &imageViews, VkExtent2D &extent) : _device(device)
{
    _swapChainFramebuffers.resize(imageViews.size());
    for (size_t i = 0; i < imageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            imageViews[i].getPrimitive(),
            depthResources.getImageView().getPrimitive()
        };
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = graphicsPipeline->getRenderPass()->getPrimitive();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;
        if (vkCreateFramebuffer(_device->getPrimitive(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create framebuffer!");
    }
}

FrameBuffers::~FrameBuffers()
{
    if (_swapChainFramebuffers.empty())
        return;
    for (auto &framebuffer : _swapChainFramebuffers)
        vkDestroyFramebuffer(_device->getPrimitive(), framebuffer, nullptr);
}

std::vector<VkFramebuffer> &FrameBuffers::getSwapChainFramebuffers()
{
    return _swapChainFramebuffers;
}
