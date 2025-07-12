#pragma once
#include "pch.h"

namespace Core
{
    enum class SeverityLevel
    {
        LOG = 0,
        DISPLAY,
        INFO,
        WARNING,
        ERROR,
        FATAL,
    };

    class Logger
    {
    public:
        static void SetSeverityLevel(SeverityLevel Severity);
        static SeverityLevel GetSeverityLevel();

        static void Log(SeverityLevel Severity, const std::string& Message);
        static void Display(SeverityLevel Severity, const std::string& Message);
        static void Info(SeverityLevel Severity, const std::string& Message);
        static void Warning(
            SeverityLevel Severity, const std::string& Message, const char* File, int Line);
        static void Error(
            SeverityLevel Severity, const std::string& Message, const char* File, int Line);
        static void Fatal(
            SeverityLevel Severity, const std::string& Message, const char* File, int Line);

    private:
        static void Output(SeverityLevel Severity, const std::string& Label,
            const std::string& Message, const char* File, int Line);

        static const char* GetColor(SeverityLevel Severity);
        static std::string GetLabel(SeverityLevel Severity);
        static SeverityLevel sm_SeverityLevel;
    };
}  // namespace Core

#ifdef _DEBUG
#define DEBUG_ASSERT(Condition, ...) \
    if (!(Condition))                \
    {                                \
        DEBUG_FATAL(##__VA_ARGS__);  \
    }

#define DEBUG_LOG(...) \
    Core::Logger::Log(Core::SeverityLevel::LOG, Core::Utility::Format(##__VA_ARGS__))

#define DEBUG_DISPLAY(...) \
    Core::Logger::Display(Core::SeverityLevel::DISPLAY, Core::Utility::Format(##__VA_ARGS__))

#define DEBUG_INFO(...) \
    Core::Logger::Info(Core::SeverityLevel::INFO, Core::Utility::Format(##__VA_ARGS__))

#define DEBUG_WARNING(...) \
    Core::Logger::Warning( \
        Core::SeverityLevel::WARNING, Core::Utility::Format(##__VA_ARGS__), __FILE__, __LINE__)

#define DEBUG_ERROR(...) \
    Core::Logger::Error( \
        Core::SeverityLevel::ERROR, Core::Utility::Format(##__VA_ARGS__), __FILE__, __LINE__)

#define DEBUG_FATAL(...) \
    Core::Logger::Fatal( \
        Core::SeverityLevel::FATAL, Core::Utility::Format(##__VA_ARGS__), __FILE__, __LINE__)
#else
#define DEBUG_LOG(...)
#define DEBUG_DISPLAY(...)
#define DEBUG_INFO(...)
#define DEBUG_WARNING(...)
#define DEBUG_ERROR(...)
#define DEBUG_FATAL(...)
#endif
