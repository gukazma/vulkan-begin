#include "VulkanShaderModule.h"
#include <iostream>
#include <stdexcept>

namespace VulkanLib {
    VulkanShaderModule::VulkanShaderModule(std::shared_ptr<VulkanDevice> device, const uint32_t* code, uint32_t size)
        : m_Device(device->m_Device)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = size;
        createInfo.pCode = code;

        if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        vkDestroyShaderModule(m_Device, m_ShaderModule, nullptr);
    }
}