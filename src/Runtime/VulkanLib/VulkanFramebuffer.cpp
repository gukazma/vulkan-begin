#include "VulkanFramebuffer.h"
#include <stdexcept>

namespace VulkanLib {
    VulkanFramebuffer::VulkanFramebuffer(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapChain> swapchain, std::shared_ptr<VulkanRenderPass> renderpass)
        : m_Device(device->m_Device), m_SwapChainImageViews(swapchain->m_SwapChainImageViews), m_RenderPass(renderpass->m_RenderPass), m_SwapChainExtent(swapchain->m_SwapChainExtent)
    {
        m_Framebuffers.resize(m_SwapChainImageViews.size());
        for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                m_SwapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = m_SwapChainExtent.width;
            framebufferInfo.height = m_SwapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        for (auto framebuffer : m_Framebuffers)
        {
            vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
        }
    }
}