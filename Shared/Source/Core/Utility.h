#pragma once
#include "pch.h"

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

    inline void Printf() {};

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

    inline std::string LoadFile(const char* Filename)
    {
        std::ifstream File;
        std::stringstream Stream;

        File.exceptions(std::ifstream::badbit | std::ifstream::failbit);

        try
        {
            File.open(Filename);
            Stream << File.rdbuf();
            File.close();
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(Utility::Format("Failed to load file at %s", Filename));
        }

        return Stream.str();
    }
}  // namespace Utility

#define PRINT_ERROR(...)                     \
    {                                        \
        Utility::Print("\033[091m[ERROR] "); \
        Utility::Printf(__VA_ARGS__);        \
        Utility::Print("\033[0m\n");         \
    }

#define CHECK(Condition, ...)                                   \
    if (!(bool)(Condition))                                     \
    {                                                           \
        throw std::runtime_error(Utility::Format(__VA_ARGS__)); \
    }