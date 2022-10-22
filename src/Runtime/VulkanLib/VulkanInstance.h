#pragma once
#include <cpp-utilities/singleton.hpp>
#include <stdint.h>
#include <vector>
#include <set>
#include <string>
#include <optional>
#include <vulkan/vulkan_core.h>
#include "GLFW/glfw3.h"
#include "VulkanLib/VulkanLog.h"
namespace VulkanLib {
    class VulkanInstance : public PublicSingleton<VulkanInstance>
    {
    public:
        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> fromats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool isComplete() {
                return	graphicsFamily.has_value() && presentFamily.has_value();
            }
        };

    public:
        VulkanInstance() = default;
        ~VulkanInstance() = default;
        void create(const char* pAppName, uint32_t pAppVersion, const char* pEngineName, uint32_t pEngineVersion, uint32_t pVulkanAPIVersion,const std::vector<const char*> pExtensions);
        void create(const std::vector<const char*> pExtensions);
        VkInstance getVKHandle();
        VkSurfaceKHR getVKSurfaceHandle();
        VkPhysicalDevice getVKPhyscialDeviceHandle();
        void destory();
        bool setupGLFWSurface(GLFWwindow* pWindow);

        VkPhysicalDevice pickPhysicalDevice(std::vector<const char*> pDeviceExtensions);

    private:
        bool isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        bool isCreated = false;
        VkInstance m_VulkanInstance;
        VkSurfaceKHR m_Surface;
        VkPhysicalDevice m_PhysicalDevice;

        std::vector<const char*> m_Extensions;
        std::vector<const char*> m_DeviceExtensions;
    };
}