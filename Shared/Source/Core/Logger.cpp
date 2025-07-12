#include "Logger.h"

namespace Core
{
    SeverityLevel Logger::sm_SeverityLevel = SeverityLevel::LOG;

    void Logger::SetSeverityLevel(SeverityLevel Severity)
    {
        sm_SeverityLevel = Severity;
    }

    SeverityLevel Logger::GetSeverityLevel()
    {
        return sm_SeverityLevel;
    }

    void Logger::Log(SeverityLevel Severity, const std::string& Message)
    {
        Output(Severity, "LOG", Message, "", 0);
    }

    void Logger::Display(SeverityLevel Severity, const std::string& Message)
    {
        Output(Severity, "DISPLAY", Message, "", 0);
    }

    void Logger::Info(SeverityLevel Severity, const std::string& Message)
    {
        Output(Severity, "INFO", Message, "", 0);
    }

    void Logger::Warning(
        SeverityLevel Severity, const std::string& Message, const char* File, int Line)
    {
        Output(Severity, "WARNING", Message, File, Line);
    }

    void Logger::Error(
        SeverityLevel Severity, const std::string& Message, const char* File, int Line)
    {
        Output(Severity, "ERROR", Message, File, Line);
    }

    void Logger::Fatal(
        SeverityLevel Severity, const std::string& Message, const char* File, int Line)
    {
        Output(Severity, "FATAL", Message, File, Line);
        std::abort();
    }

    void Logger::Output(SeverityLevel Severity, const std::string& Label,
        const std::string& Message, const char* File, int Line)
    {
        if (Severity < sm_SeverityLevel)
            return;

        std::cout << GetColor(Severity) << "[" << Label << "] " << Message;

        if (File && File[0] != '\0')
            std::cout << " (" << File << ":" << Line << ")";

        std::cout << "\033[0m\n";
    }

    const char* Logger::GetColor(SeverityLevel Severity)
    {
        switch (Severity)
        {
        case SeverityLevel::INFO:
            return "\033[94m";
        case SeverityLevel::WARNING:
            return "\033[93m";
        case SeverityLevel::ERROR:
            return "\033[91m";
        case SeverityLevel::FATAL:
            return "\033[31m";
        case SeverityLevel::DISPLAY:
            return "\033[92m";
        default:
            return "\033[0m";
        }
    }

    std::string Logger::GetLabel(SeverityLevel Severity)
    {
        switch (Severity)
        {
        case SeverityLevel::LOG:
            return "LOG";
        case SeverityLevel::DISPLAY:
            return "DISPLAY";
        case SeverityLevel::INFO:
            return "INFO";
        case SeverityLevel::WARNING:
            return "WARNING";
        case SeverityLevel::ERROR:
            return "ERROR";
        case SeverityLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }
}  // namespace Core