#pragma once

#include "VulkanLib/VulkanDevice.h"
#include "VulkanLib/VulkanPipelineShaderStage.h"
#include "VulkanLib/VulkanRenderPass.h"
#include "VulkanLib/VulkanSwapChain.h"
#include <memory>
#include <vulkan/vulkan_core.h>
namespace VulkanLib {
    class VulkanGraphicsPipline
    {
    public:
        VulkanGraphicsPipline(std::shared_ptr<VulkanDevice> vulkanDevice, std::shared_ptr<VulkanRenderPass> vulkanRenderPass, VulkanLib::VulkanPipelineShaderStage shaderstage, std::shared_ptr<VulkanSwapChain> vulkanSwapChain);
        ~VulkanGraphicsPipline();
    public:
        VkDevice m_Device;
        VkExtent2D m_SwapChainExtent;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_GraphicsPipeline;
        VkRenderPass m_RenderPass;
    };
}