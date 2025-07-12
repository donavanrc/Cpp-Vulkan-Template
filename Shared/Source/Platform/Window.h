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

    class Window
    {
    public:
        static Window* GetInstance() { return sm_Instance; }

        Window(const WindowDesc& Desc);
        void Initialize();
        void Destroy();
        void ProcessEvents();
        bool WantsExit();
        bool IsFullscreen() const { return m_Fullscreen; }
        void SetFullscreen(bool Fullscreen);
        GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }
        WindowDesc GetDesc() const { return m_Desc; }

    private:
        static Window* sm_Instance;

        GLFWwindow* m_WindowHandle;
        WindowDesc m_Desc;
        bool m_Fullscreen;
    };
}  // namespace Platform
