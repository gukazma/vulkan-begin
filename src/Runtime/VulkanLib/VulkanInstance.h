#pragma once
#include <cpp-utilities/singleton.hpp>
#include <stdint.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "GLFW/glfw3.h"
#include "VulkanLib/VulkanLog.h"
namespace VulkanLib {
    class VulkanInstance : public PublicSingleton<VulkanInstance>
    {
    public:
        VulkanInstance() = default;
        ~VulkanInstance() = default;
        void Create(const char* pAppName, uint32_t pAppVersion, const char* pEngineName, uint32_t pEngineVersion, uint32_t pVulkanAPIVersion,const std::vector<const char*> pExtensions);
        void Create(const std::vector<const char*> pExtensions);
        VkInstance GetVKHandle();
        VkSurfaceKHR GetVKSurfaceHandle();
        void Destory();
        bool SetupGLFWSurface(GLFWwindow* pWindow);
    private:
        void PickPhysicalDevice();
        bool isCreated = false;
        VkInstance m_VulkanInstance;
        VkSurfaceKHR m_Surface;

        std::vector<const char*> m_Extensions;
    };
}