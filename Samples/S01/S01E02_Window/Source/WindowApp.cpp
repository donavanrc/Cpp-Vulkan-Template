#include "WindowApp.h"

void WindowApp::Initialize()
{
    Platform::WindowDesc WindowDesc = {};
    WindowDesc.Title = "WindowApp";
    WindowDesc.Width = 800;
    WindowDesc.Height = 600;
    WindowDesc.Fullscreen = true;

    m_Window = Platform::Window(WindowDesc);
    m_Window.SetEventHandler(this);
    m_Window.Initialize();
}

void WindowApp::Destroy()
{
    m_Window.Destroy();
}

void WindowApp::Run()
{
    m_Running = true;

    while (m_Running)
    {
        m_Window.ProcessEvents();

        if (Input::IsKeyDown(GLFW_KEY_ESCAPE))
        {
            m_Running = false;
        }

        if (Input::IsKeyDown(GLFW_KEY_F11))
        {
            m_Window.SetFullscreen(!m_Window.IsFullscreen());
        }
    }

    DEBUG_LOG("Goodbye!");
}

void WindowApp::OnWindowClose()
{
    m_Running = false;
}

void WindowApp::OnWindowResize(uint32_t Width, uint32_t Height)
{
    DEBUG_INFO("Window resized (%d:%d)", Width, Height);
};

START_APPLICATION(WindowApp);