#include "VulkanInstance.h"
#include <iostream>
#include <vulkan/vulkan_core.h>
namespace VulkanLib {
    void VulkanInstance::Create(const char* pAppName, uint32_t pAppVersion, const char* pEngineName, uint32_t pEngineVersion, uint32_t pVulkanAPIVersion,const std::vector<const char*> pExtensions)
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = pAppName;
        appInfo.applicationVersion = pAppVersion;
        appInfo.pEngineName = "VulkanLib";
        appInfo.engineVersion = pEngineVersion;
        appInfo.apiVersion = pVulkanAPIVersion;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(pExtensions.size());
        createInfo.ppEnabledExtensionNames = pExtensions.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
            return;
        }
        isCreated = true;
    }

    void VulkanInstance::Create(const std::vector<const char*> pExtensions)
    {
        VkApplicationInfo appInfo                    = {};
                          appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                          appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                          appInfo.pApplicationName   = "Vulkan Aplication";
                          appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
                          appInfo.pEngineName        = "No Engine";
                          appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
                          appInfo.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo                         = {};
                             createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                             createInfo.pApplicationInfo        = &appInfo;
                             createInfo.enabledLayerCount       = 0;
                             createInfo.enabledExtensionCount   = static_cast<uint32_t>(pExtensions.size());
                             createInfo.ppEnabledExtensionNames = pExtensions.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vulkan instance!");
            return;
        }
        isCreated = true;
    }

    void VulkanInstance::Destory()
    {
        if (!isCreated) VULKAN_ERROR("std::string_view msg");
    }

    VkInstance VulkanInstance::GetVKHandle() 
    {
        if (!isCreated) 
        {
            VULKAN_ERROR("Vulkan didn't initialize");
            return VK_NULL_HANDLE;
        }
        return m_VulkanInstance;
    }

    bool VulkanInstance::SetupGLFWSurface(GLFWwindow* pWindow)
    {
        return glfwCreateWindowSurface(m_VulkanInstance, pWindow, nullptr, &m_Surface);
    }
    VkSurfaceKHR VulkanInstance::GetVKSurfaceHandle()
    {
        return m_Surface;
    }

}