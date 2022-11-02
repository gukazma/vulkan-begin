#pragma once
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace VulkanLib {
    class VulkanDevice
    {
    public:
        VulkanDevice(std::set<uint32_t> pUniqueQueueFamilies, const std::vector<const char*> pDeviceExtensions);
        VulkanDevice()=default;
        ~VulkanDevice();

        VkCommandPool getCommandPool();

        std::vector<const char*> m_DeviceExtensions;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
        VkCommandPool m_CommandPool;
    };
}