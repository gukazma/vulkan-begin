#include "VulkanLog.h"
#include <iostream>
#include <string>

namespace VulkanLib {

    std::string CastLogLevel(LogLevel level)
    {
        std::string rnt;
        switch (level) {
            case LogLevel::Info:
                rnt = "[Vulkan Info]: ";
            case LogLevel::Warn:
                rnt = "[Vulkan Warn]: ";
            case LogLevel::Error:
                rnt = "[Vulkan Error]: ";
            default:
                rnt = "[Vulkan Info]: ";
        };
        return rnt;
    }
    void Log(LogLevel level, const char* msg)
    {
        std::cout << CastLogLevel(level) << msg << std::endl;
    }
}

