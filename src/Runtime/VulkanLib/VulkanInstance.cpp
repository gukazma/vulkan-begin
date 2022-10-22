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
    VkPhysicalDevice VulkanInstance::GetVKPhyscialDeviceHandle()
    {
        return m_PhysicalDevice;
    }

    VkPhysicalDevice VulkanInstance::PickPhysicalDevice(std::vector<const char*> pDeviceExtensions)
    {
        m_DeviceExtensions = pDeviceExtensions;
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                m_PhysicalDevice = device;
                return m_PhysicalDevice;
            }
        }
    }

    bool VulkanInstance::isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);

            swapChainAdequate = !swapChainSupport.fromats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    VulkanInstance::QueueFamilyIndices VulkanInstance::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

            if (presentSupport)
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }
    // **************************** setup validation layer *******************************
    bool VulkanInstance::checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }
    // **************************** create swap chain ****************************
    VulkanInstance::SwapChainSupportDetails VulkanInstance::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        // get surface capabilities ( max/min image )
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

        // get surface format (RGB...)
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.fromats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.fromats.data());
        }

        // get surface present mode (imediate / fifo ...)
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }
}