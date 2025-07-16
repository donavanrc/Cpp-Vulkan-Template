#pragma once
#include "pch.h"

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
    static void Output(SeverityLevel Severity, const std::string& Label, const std::string& Message,
        const char* File, int Line);

    static const char* GetColor(SeverityLevel Severity);
    static std::string GetLabel(SeverityLevel Severity);
    static SeverityLevel sm_SeverityLevel;
};

#ifdef _DEBUG
#define DEBUG_ASSERT(Condition, ...) \
    if (!(Condition))                \
    {                                \
        DEBUG_FATAL(##__VA_ARGS__);  \
    }

#define DEBUG_LOG(...) Logger::Log(SeverityLevel::LOG, Utility::Format(##__VA_ARGS__))

#define DEBUG_DISPLAY(...) Logger::Display(SeverityLevel::DISPLAY, Utility::Format(##__VA_ARGS__))

#define DEBUG_INFO(...) Logger::Info(SeverityLevel::INFO, Utility::Format(##__VA_ARGS__))

#define DEBUG_WARNING(...) \
    Logger::Warning(SeverityLevel::WARNING, Utility::Format(##__VA_ARGS__), __FILE__, __LINE__)

#define DEBUG_ERROR(...) \
    Logger::Error(SeverityLevel::ERROR, Utility::Format(##__VA_ARGS__), __FILE__, __LINE__)

#define DEBUG_FATAL(...) \
    Logger::Fatal(SeverityLevel::FATAL, Utility::Format(##__VA_ARGS__), __FILE__, __LINE__)
#else
#define DEBUG_LOG(...)
#define DEBUG_DISPLAY(...)
#define DEBUG_INFO(...)
#define DEBUG_WARNING(...)
#define DEBUG_ERROR(...)
#define DEBUG_FATAL(...)
#endif
