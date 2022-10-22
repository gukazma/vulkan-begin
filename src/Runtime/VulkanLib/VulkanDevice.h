#pragma once

namespace VulkanLib {
    class VulkanDevice
    {
    public:
        static VulkanDevice* create();
    private:
        VulkanDevice();
        ~VulkanDevice();
    };
}