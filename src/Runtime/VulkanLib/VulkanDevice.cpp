#include "VulkanDevice.h"
#include "VulkanLib/VulkanInstance.h"
#include "VulkanLib/VulkanLog.h"
#include <stdexcept>

namespace VulkanLib {
    VulkanDevice::VulkanDevice(std::set<uint32_t> pUniqueQueueFamilies, const std::vector<const char*> pDeviceExtensions)
        :m_DeviceExtensions(pDeviceExtensions)
    {
        if (!VulkanLib::VulkanInstance::getInstance()->isCreated) 
        {
            VULKAN_ERROR("Vulkan instance has not created!");
            return;
        }

        m_PhysicalDevice = VulkanInstance::getInstance()->pickPhysicalDevice(pDeviceExtensions);

        VulkanInstance::QueueFamilyIndices indices = VulkanInstance::getInstance()->findQueueFamilies(m_PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (auto queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }


        VkPhysicalDeviceFeatures deviceFeatures = {};
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
        createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
    }

    VulkanDevice::~VulkanDevice()
    {

    }

}