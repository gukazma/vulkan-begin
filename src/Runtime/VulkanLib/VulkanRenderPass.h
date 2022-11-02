#pragma once

#include <vulkan/vulkan_core.h>
namespace VulkanLib {
    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VkDevice device, VkFormat format);
        ~VulkanRenderPass();

        VkRenderPass getVkHandle()
        {
            return m_RenderPass;
        }
    public:
        VkRenderPass m_RenderPass;
        VkDevice m_Device;
    };
}