#pragma once

namespace VulkanLib {
    enum class LogLevel
    {
        Info, Warn, Error
    };
    void Log(LogLevel level, const char* msg);
}

#define VULKAN_Info(msg)    VulkanLib::Log(LogLevel::Info, msg)
#define VULKAN_WARN(msg)    VulkanLib::Log(LogLevel::Warn, msg)
#define VULKAN_ERROR(msg)   VulkanLib::Log(LogLevel::Error, msg)