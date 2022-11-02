#pragma once

#include "VulkanLib/VulkanDevice.h"
#include <memory>
#include <vulkan/vulkan_core.h>
namespace VulkanLib {
    class VulkanShaderModule
    {
    public:
        VulkanShaderModule(std::shared_ptr<VulkanDevice> device, const uint32_t* code, uint32_t size);
        ~VulkanShaderModule();
    public:
        VkShaderModule m_ShaderModule;
        VkDevice m_Device;
    };
}