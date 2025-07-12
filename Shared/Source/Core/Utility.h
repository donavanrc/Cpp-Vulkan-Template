#pragma once
#include "pch.h"

namespace Core
{
    namespace Utility
    {
        inline void Print(const char* Message)
        {
            std::cout << Message;
        }

        inline void Printf(const char* Format, ...)
        {
            char Buffer[256];
            va_list Ap;
            va_start(Ap, Format);
            vsprintf_s(Buffer, 256, Format, Ap);
            va_end(Ap);
            Print(Buffer);
        }

        inline std::string Format(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            std::string result;
            result.resize(vsnprintf(nullptr, 0, format, args));
            vsnprintf(result.data(), result.size() + 1, format, args);
            va_end(args);
            return result;
        }

        inline std::string Format()
        {
            return "";
        };
    }  // namespace Utility
}  // namespace Core

#define PRINT_ERROR(...)                           \
    {                                              \
        Core::Utility::Print("\033[091m[ERROR] "); \
        Core::Utility::Printf(__VA_ARGS__);        \
        Core::Utility::Print("\033[0m\n");         \
    }

#define CHECK(Condition, ...)                                         \
    if (!(bool)(Condition))                                           \
    {                                                                 \
        throw std::runtime_error(Core::Utility::Format(__VA_ARGS__)); \
    }