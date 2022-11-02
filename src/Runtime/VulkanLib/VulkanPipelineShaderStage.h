#pragma once

#include "VulkanLib/VulkanShaderModule.h"
#include <initializer_list>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace VulkanLib {
    class VulkanPipelineShaderStage
    {
        struct ShaderInfo
        {
            std::shared_ptr<VulkanShaderModule> shaderModule;
            VkShaderStageFlagBits shaderFlag;
            const char* shaderEntryPoint;
        };
    public:
        VulkanPipelineShaderStage(std::initializer_list<ShaderInfo> shaderInfos);
        ~VulkanPipelineShaderStage();

        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageInfos;
    };
}