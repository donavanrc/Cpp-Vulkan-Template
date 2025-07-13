#include "WindowApp.h"

void WindowApp::Initialize()
{
    m_WindowDesc.Title = "WindowApp";
    m_WindowDesc.Width = 800;
    m_WindowDesc.Height = 600;

    m_Window = std::unique_ptr<Platform::Window>(new Platform::Window(m_WindowDesc));
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

    m_Timer.Reset();

    while (m_Running)
    {
        m_Window->ProcessEvents();

        m_Timer.Update();
        CalculateFrameStats();

        if (Input::IsKeyDown(GLFW_KEY_ESCAPE))
        {
            m_Running = false;
        }

        if (Input::IsKeyDown(GLFW_KEY_F11))
        {
            m_Window->SetFullscreen(!m_Window->IsFullscreen());
        }

        if (Input::IsMouseDown(GLFW_MOUSE_BUTTON_LEFT))
        {
            auto Position = Input::GetMousePosition();
            DEBUG_INFO("MouseX=%f MouseY=%f", Position.X, Position.Y);
        }
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

void WindowApp::CalculateFrameStats()
{
    m_FrameCount++;
    m_ElapsedTime += m_Timer.GetDeltaTime();

    if (m_ElapsedTime >= 1000.0f)
    {
        float FrameTime = 1000.0f / m_FrameCount;

        std::string WindowTitle = Core::Utility::Format("%s | Rate: %u fps, Time: %.2f ms",
            m_WindowDesc.Title.c_str(), m_FrameCount, FrameTime);

        m_Window->SetTitle(WindowTitle.c_str());

        m_FrameCount = 0;
        m_ElapsedTime = 0;
    }
}

START_APPLICATION(WindowApp);