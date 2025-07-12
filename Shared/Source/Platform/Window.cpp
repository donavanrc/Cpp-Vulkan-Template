#include "Window.h"

namespace Platform
{
    Window* Window::sm_Instance = nullptr;

    Window::Window(const WindowDesc& Desc)
        : m_Desc(Desc), m_WindowHandle(nullptr), m_Fullscreen(false)
    {
        sm_Instance = this;

        glfwSetErrorCallback(
            [](int Error, const char* Description) { DEBUG_ERROR("%d: %s", Error, Description); });

        if (!glfwInit())
        {
            throw std::runtime_error("Failed initialize window");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

    void Window::Initialize()
    {
        DEBUG_ASSERT(m_WindowHandle == nullptr);

        m_WindowHandle =
            glfwCreateWindow(m_Desc.Width, m_Desc.Height, m_Desc.Title.c_str(), nullptr, nullptr);

        if (!m_WindowHandle)
        {
            throw std::runtime_error("Failed create window");
        }

        DEBUG_DISPLAY(
            "Window created \"%s\" (%d:%d)", m_Desc.Title.c_str(), m_Desc.Width, m_Desc.Height);

        glfwFocusWindow(m_WindowHandle);

        SetFullscreen(m_Desc.Fullscreen);

        glfwShowWindow(m_WindowHandle);
    }

    void Window::Destroy()
    {
        if (m_WindowHandle)
        {
            glfwDestroyWindow(m_WindowHandle);
            m_WindowHandle = nullptr;
        }
        glfwTerminate();
    }

    void Window::ProcessEvents()
    {
        DEBUG_ASSERT(m_WindowHandle);

        glfwPollEvents();
    }

    bool Window::WantsExit()
    {
        DEBUG_ASSERT(m_WindowHandle);

        return glfwWindowShouldClose(m_WindowHandle);
    }

    void Window::SetFullscreen(bool Fullscreen)
    {
        DEBUG_ASSERT(m_WindowHandle);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        m_Fullscreen = Fullscreen;

        auto Monitor = glfwGetPrimaryMonitor();

        glfwSetWindowMonitor(m_WindowHandle, m_Fullscreen ? Monitor : nullptr, 0, 0, m_Desc.Width,
            m_Desc.Height, GLFW_DONT_CARE);

        if (!m_Fullscreen)
        {
            auto* Mode = glfwGetVideoMode(Monitor);
            int PositionX = (Mode->width - m_Desc.Width) / 2;
            int PositionY = (Mode->height - m_Desc.Height) / 2;
            glfwSetWindowPos(m_WindowHandle, PositionX, PositionY);
        }
    }
}  // namespace Platform
