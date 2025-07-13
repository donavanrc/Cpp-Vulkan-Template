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

        CHECK(glfwInit(), "Failed to initialize window");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

    void Window::Initialize()
    {
        DEBUG_ASSERT(m_EventHandler);
        DEBUG_ASSERT(m_WindowHandle == nullptr);

        m_WindowHandle =
            glfwCreateWindow(m_Desc.Width, m_Desc.Height, m_Desc.Title.c_str(), nullptr, nullptr);

        CHECK(m_WindowHandle, "Failed to create window");

        DEBUG_DISPLAY(
            "Window created \"%s\" (%d:%d)", m_Desc.Title.c_str(), m_Desc.Width, m_Desc.Height);

        glfwFocusWindow(m_WindowHandle);

        SetFullscreen(m_Desc.Fullscreen);

        glfwSetWindowUserPointer(m_WindowHandle, m_EventHandler);

        glfwSetWindowCloseCallback(m_WindowHandle,
            [](GLFWwindow* Window)
            {
                auto EventHandler =
                    static_cast<WindowEventHandler*>(glfwGetWindowUserPointer(Window));
                EventHandler->OnWindowClose();
            });

        glfwSetFramebufferSizeCallback(m_WindowHandle,
            [](GLFWwindow* Window, int Width, int Height)
            {
                auto EventHandler =
                    static_cast<WindowEventHandler*>(glfwGetWindowUserPointer(Window));
                EventHandler->OnWindowResize(Width, Height);
            });

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

    inline GLFWmonitor* GetCurrentMonitor(GLFWwindow* window)
    {
        int SelectedOverlap = 0;
        GLFWmonitor* SelectedMonitor = nullptr;

        int WindowX, WindowY, WindowWidth, WindowHeight;
        glfwGetWindowPos(window, &WindowX, &WindowY);
        glfwGetWindowSize(window, &WindowWidth, &WindowHeight);

        int MonitorCount;
        GLFWmonitor** Monitors;
        Monitors = glfwGetMonitors(&MonitorCount);

        for (uint32_t Index = 0; Index < MonitorCount; ++Index)
        {
            auto Monitor = Monitors[Index];
            const GLFWvidmode* mode = glfwGetVideoMode(Monitor);

            int MonitorX, MonitorY;
            glfwGetMonitorPos(Monitor, &MonitorX, &MonitorY);

            int ModeWidth = mode->width;
            int ModeHeight = mode->height;

            int Overlap = std::max(0, std::min(WindowX + WindowWidth, MonitorX + ModeWidth) -
                                          std::max(WindowX, MonitorX)) *
                          std::max(0, std::min(WindowY + WindowHeight, MonitorY + ModeHeight) -
                                          std::max(WindowY, MonitorY));

            if (SelectedOverlap < Overlap)
            {
                SelectedOverlap = Overlap;
                SelectedMonitor = Monitor;
            }
        }

        return SelectedMonitor;
    }

    void Window::SetFullscreen(bool Fullscreen)
    {
        DEBUG_ASSERT(m_WindowHandle);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        m_Fullscreen = Fullscreen;

        glfwSetWindowAttrib(m_WindowHandle, GLFW_DECORATED, m_Fullscreen ? GLFW_FALSE : GLFW_TRUE);

        GLFWmonitor* Monitor = GetCurrentMonitor(m_WindowHandle);

        if (Monitor == nullptr)
        {
            Monitor = glfwGetPrimaryMonitor();
        }

        const GLFWvidmode* Mode = glfwGetVideoMode(Monitor);

        if (m_Fullscreen)
        {
            glfwSetWindowSize(m_WindowHandle, Mode->width, Mode->height);
            glfwSetWindowPos(m_WindowHandle, 0, 0);
        }
        else
        {
            glfwSetWindowSize(m_WindowHandle, m_Desc.Width, m_Desc.Height);
            int posX = (Mode->width - m_Desc.Width) / 2;
            int posY = (Mode->height - m_Desc.Height) / 2;
            glfwSetWindowPos(m_WindowHandle, posX, posY);
        }
    }

    void Window::SetTitle(const std::string& Title)
    {
        DEBUG_ASSERT(m_WindowHandle);
        m_Title = Title;
        glfwSetWindowTitle(m_WindowHandle, m_Title.c_str());
    }

}  // namespace Platform
