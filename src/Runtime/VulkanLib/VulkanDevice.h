#pragma once

namespace VulkanLib {
    class VulkanDevice
    {
    public:
        VulkanDevice();
        ~VulkanDevice();

        static VulkanDevice create();
    };
}