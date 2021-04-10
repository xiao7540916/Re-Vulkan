#pragma once

#include "Horizon/Core/HorizonCommon.h"

HORIZON_DISABLE_WARNINGS()
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
HORIZON_ENABLE_WARNINGS()

namespace Horizon
{

    using LoggerSharedPtr = std::shared_ptr<spdlog::logger>;

#ifndef LOG_TRACE
#define LOG_TRACE(...)     LogManager::GetLogger()->trace(__VA_ARGS__)
#endif
#ifndef LOG_DEBUG
#define LOG_DEBUG(...)     LogManager::GetLogger()->debug(__VA_ARGS__)
#endif
#ifndef LOG_INFO
#define LOG_INFO(...)      LogManager::GetLogger()->info(__VA_ARGS__)
#endif
#ifndef LOG_WARNING
#define LOG_WARNING(...)   LogManager::GetLogger()->warn(__VA_ARGS__)
#endif
#ifndef LOG_ERROR
#define LOG_ERROR(...)     LogManager::GetLogger()->error(__VA_ARGS__)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(...)     LogManager::GetLogger()->critical(__VA_ARGS__)
#endif

    class LogManager
    {
    public:
        enum class MessageLevel
        {
            Trace = 0,
            Debug = 1,
            Info = 2,
            Warning = 3,
            Error = 4,
            Fatal = 5,
        };
        static void Init();
        static LoggerSharedPtr GetLogger() { return kLogger; }
    private:
        LogManager() = delete;
        static LoggerSharedPtr kLogger;
    };
}