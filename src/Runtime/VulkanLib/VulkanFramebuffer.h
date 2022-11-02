#pragma once

#include "VulkanLib/VulkanDevice.h"
#include "VulkanLib/VulkanRenderPass.h"
#include "VulkanLib/VulkanSwapChain.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace VulkanLib {
     class VulkanFramebuffer
     {
        public:
            VulkanFramebuffer(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapChain> swapchain, std::shared_ptr<VulkanRenderPass> renderpass);
            ~VulkanFramebuffer();
        public:
            std::vector<VkFramebuffer> m_Framebuffers;
            std::vector<VkImageView> m_SwapChainImageViews;
            VkRenderPass m_RenderPass;
            VkExtent2D m_SwapChainExtent;
            VkDevice m_Device;
     };
}