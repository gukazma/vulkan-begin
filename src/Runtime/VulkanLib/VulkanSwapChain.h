#pragma once
#include "VulkanLib/VulkanDevice.h"
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace VulkanLib {
    class VulkanSwapChain
    {
    public:
        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> fromats;
            std::vector<VkPresentModeKHR> presentModes;
        };

    public:
        VulkanSwapChain(std::shared_ptr<VulkanDevice> pVulkanDevice);
        ~VulkanSwapChain()=default;
    public:
        uint32_t    m_Width;
        uint32_t    m_Height;
        VkSurfaceKHR m_Surface;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        VkSwapchainKHR m_SwapChain;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        std::shared_ptr<VulkanDevice> m_VulkanDevice;
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresenMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    private:
        void createImageViews();
    };
}