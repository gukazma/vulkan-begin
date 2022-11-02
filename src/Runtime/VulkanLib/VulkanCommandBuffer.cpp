#include "VulkanCommandBuffer.h"
#include "VulkanMacro.h"
#include <stdexcept>
namespace VulkanLib {
    VulkanCommandBuffer::VulkanCommandBuffer(std::shared_ptr<VulkanDevice> device)
        : m_Device(device->m_Device), m_CommandPool(device->getCommandPool())
    {
        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

        if (vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {

    }
}