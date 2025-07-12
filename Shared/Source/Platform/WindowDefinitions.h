#pragma once
#include "pch.h"

namespace Platform
{
    struct WindowDesc
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
        bool Fullscreen = false;
    };
}  // namespace Platform