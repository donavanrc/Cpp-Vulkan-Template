#pragma once
#include "pch.h"

namespace Utility
{
    inline void Print(const char *Message) { std::cout << Message; }

    inline void Printf(const char *Format, ...)
    {
        char Buffer[256];
        va_list Ap;
        va_start(Ap, Format);
        vsprintf_s(Buffer, 256, Format, Ap);
        va_end(Ap);
        Print(Buffer);
    }

    inline void PrintLn(const char *Message) { std::cout << Message << '\n'; }

    inline void Printf() {}
};

#ifdef _DEBUG
#define DEBUG_ASSERT(Condition, ...)                    \
    if (!(bool)(Condition))                             \
    {                                                   \
        Utility::Print("\033[031m[ASSERT] ");           \
        Utility::Printf(__VA_ARGS__);                   \
        Utility::Print("\n");                           \
        Utility::PrintLn(#Condition " is false");       \
        Utility::Printf("(%s:%d)", __FILE__, __LINE__); \
        Utility::Print("\033[0m\n");                    \
        std::abort();                                   \
    }
#define DEBUG_DISPLAY(...)                     \
    {                                          \
        Utility::Print("\033[092m[DISPLAY] "); \
        Utility::Printf(__VA_ARGS__);          \
        Utility::Print("\033[0m\n");           \
    }
#define DEBUG_ERROR(...)                                  \
    {                                                     \
        Utility::Print("\033[091m[ERROR] ");              \
        Utility::Printf(__VA_ARGS__);                     \
        Utility::Printf("\n(%s:%d)", __FILE__, __LINE__); \
        Utility::Print("\033[0m\n");                      \
    }
#define DEBUG_FATAL(...)                                  \
    {                                                     \
        Utility::Print("\033[031m[FATAL] ");              \
        Utility::Printf(__VA_ARGS__);                     \
        Utility::Printf("\n(%s:%d)", __FILE__, __LINE__); \
        Utility::Print("\033[0m\n");                      \
    }
#define DEBUG_INFO(...)                                   \
    {                                                     \
        Utility::Print("\033[094m[INFO] ");               \
        Utility::Printf(__VA_ARGS__);                     \
        Utility::Printf("\n(%s:%d)", __FILE__, __LINE__); \
        Utility::Print("\033[0m\n");                      \
    }
#define DEBUG_LOG(...)                \
    {                                 \
        Utility::Print("[LOG] ");     \
        Utility::Printf(__VA_ARGS__); \
        Utility::Print("\n");         \
    }
#define DEBUG_WARNING(...)                                \
    {                                                     \
        Utility::Print("\033[093m[WARNING] ");            \
        Utility::Printf(__VA_ARGS__);                     \
        Utility::Printf("\n(%s:%d)", __FILE__, __LINE__); \
        Utility::Print("\033[0m\n");                      \
    }
#else
#define DEBUG_ASSERT(Condition, ...)
#define DEBUG_DISPLAY(...)
#define DEBUG_ERROR(...)
#define DEBUG_FATAL(...)
#define DEBUG_INFO(...)
#define DEBUG_LOG(...)
#define DEBUG_WARNING(...)
#endif // _DEBUG