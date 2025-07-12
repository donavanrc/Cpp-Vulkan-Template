#pragma once
#include "pch.h"
#include "Platform/WindowEventHandler.h"

namespace Core
{
    class Application : public Platform::WindowEventHandler
    {
    public:
        virtual void Initialize() = 0;
        virtual void Destroy() = 0;
        virtual void Run() = 0;
    };
}  // namespace Core