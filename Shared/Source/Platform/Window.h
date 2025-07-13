#pragma once
#include "pch.h"
#include "WindowDefinitions.h"
#include "WindowEventHandler.h"

namespace Platform
{
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
        void SetEventHandler(WindowEventHandler* EventHandler) { m_EventHandler = EventHandler; }
        GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }
        WindowDesc GetDesc() const { return m_Desc; }
        std::string GetTitle() const { return m_Title; }
        void SetTitle(const std::string& Title);

    private:
        static Window* sm_Instance;

        GLFWwindow* m_WindowHandle;
        WindowDesc m_Desc;
        bool m_Fullscreen;
        WindowEventHandler* m_EventHandler;
        std::string m_Title;
    };

    typedef void (*WindowCloseFunc)(Platform::Window* Window);
    typedef void (*WindowResizeFunc)(Platform::Window* Window, uint32_t Width, uint32_t Height);
}  // namespace Platform
