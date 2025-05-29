#ifndef DESCRIPTORSETS_HPP_
    #define DESCRIPTORSETS_HPP_

    #include "DescriptorSetLayout.hpp"
    #include "DescriptorPool.hpp"

class DescriptorSets {
    public:
        DescriptorSets(Device &device, DescriptorSetLayout &descriptorSetLayout, DescriptorPool &descriptorPool);
        ~DescriptorSets();
        std::vector<VkDescriptorSet> &getPrimitive();
        void write(uint32_t index, VkDescriptorBufferInfo &bufferInfo, VkDescriptorImageInfo &imageInfo);

    protected:
    private:
        std::vector<VkDescriptorSet> _descriptorSets;
        Device &_device;
};

#endif
