#pragma once

#include "VulkanLib/VulkanDevice.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace VulkanLib {
    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer(std::shared_ptr<VulkanDevice> device);
        ~VulkanCommandBuffer();
    public:
        VkDevice m_Device;
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;
    };
}