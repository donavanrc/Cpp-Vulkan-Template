#include "WindowApp.h"

void WindowApp::Initialize()
{
    Platform::WindowDesc WindowDesc = {};
    WindowDesc.Title = "WindowApp";
    WindowDesc.Width = 800;
    WindowDesc.Height = 600;

    m_Window = std::unique_ptr<Platform::Window>(new Platform::Window(WindowDesc));
    m_Window->SetEventHandler(this);
    m_Window->Initialize();
}

void WindowApp::Destroy()
{
    m_Window->Destroy();
}

void WindowApp::Run()
{
    m_Running = true;

    while (m_Running)
    {
        if (Input::IsKeyDown(GLFW_KEY_ESCAPE))
        {
            m_Running = false;
        }

        if (Input::IsKeyDown(GLFW_KEY_F11))
        {
            m_Window->SetFullscreen(!m_Window->IsFullscreen());
        }

        auto Position = Input::GetMousePosition();
        DEBUG_INFO("MouseX=%f MouseY=%f", Position.X, Position.Y);

        m_Window->ProcessEvents();
    }

    DEBUG_DISPLAY("Goodbye!");
}

void WindowApp::OnWindowClose()
{
    m_Running = false;
}

void WindowApp::OnWindowResize(uint32_t Width, uint32_t Height)
{
    DEBUG_DISPLAY("Window resized (%d:%d)", Width, Height);
};

START_APPLICATION(WindowApp);