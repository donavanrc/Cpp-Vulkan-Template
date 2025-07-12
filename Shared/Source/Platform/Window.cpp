#include "Window.h"

namespace Platform
{
    Window::Window(const WindowDesc &Desc)
        : m_Desc(Desc), m_WindowHandle(nullptr)
    {
        glfwSetErrorCallback([](int Error, const char *Description)
                             { DEBUG_ERROR("%d: %s", Error, Description); });

        if (!glfwInit())
        {
            throw std::runtime_error("Failed initialize window");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    void Window::Initialize()
    {
        DEBUG_ASSERT(m_WindowHandle == nullptr);

        m_WindowHandle = glfwCreateWindow(m_Desc.Width, m_Desc.Height, m_Desc.Title.c_str(), nullptr, nullptr);

        if (!m_WindowHandle)
        {
            throw std::runtime_error("Failed create window");
        }

        DEBUG_DISPLAY("Window created \"%s\" (%d:%d)", m_Desc.Title.c_str(), m_Desc.Width, m_Desc.Height);
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
}
