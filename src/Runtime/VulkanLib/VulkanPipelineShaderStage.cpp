#include "VulkanPipelineShaderStage.h"

namespace VulkanLib {

    VulkanPipelineShaderStage::VulkanPipelineShaderStage(std::initializer_list<ShaderInfo> shaderInfos)
    {
        for (auto shaderInfo : shaderInfos) {
            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = shaderInfo.shaderFlag;
            shaderStageInfo.module = shaderInfo.shaderModule->m_ShaderModule;
            shaderStageInfo.pName = shaderInfo.shaderEntryPoint;
            m_ShaderStageInfos.push_back(shaderStageInfo);
        }
    }
    VulkanPipelineShaderStage::~VulkanPipelineShaderStage()
    {

    }
}