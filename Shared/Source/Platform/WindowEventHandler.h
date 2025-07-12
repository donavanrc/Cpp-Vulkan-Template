#pragma once
#include "pch.h"

namespace Platform
{

    class WindowEventHandler
    {
    public:
        virtual void OnWindowClose() {};
        virtual void OnWindowResize(uint32_t Width, uint32_t Height) {};
    };
}  // namespace Platform
