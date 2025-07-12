#pragma once
#include "pch.h"

namespace Platform
{
    struct WindowDesc
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
    };

    class Window
    {
    public:
        Window(const WindowDesc &Desc);
        void Initialize();
        void Destroy();
        void ProcessEvents();
        bool WantsExit();
        GLFWwindow *GetWindowHandle() const { return m_WindowHandle; }
        WindowDesc GetDesc() const { return m_Desc; }

    private:
        GLFWwindow *m_WindowHandle;
        WindowDesc m_Desc;
    };
}