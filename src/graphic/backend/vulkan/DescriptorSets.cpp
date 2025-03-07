#include "DescriptorSets.hpp"

DescriptorSets::DescriptorSets(Device &device, DescriptorSetLayout &descriptorSetLayout, DescriptorPool &descriptorPool) : _device(device)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout.getPrimitive());
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool.getPrimitive();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    _descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(_device.getPrimitive(), &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
}
}

DescriptorSets::~DescriptorSets()
{
}


void DescriptorSets::write(uint32_t index, VkDescriptorBufferInfo &bufferInfo)
{
    VkDescriptorSet &descriptorSet = _descriptorSets[index];
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;
    descriptorWrite.pTexelBufferView = nullptr;
    
    vkUpdateDescriptorSets(_device.getPrimitive(), 1, &descriptorWrite, 0, nullptr);
}

std::vector<VkDescriptorSet> &DescriptorSets::getPrimitive()
{
    return _descriptorSets;
}

