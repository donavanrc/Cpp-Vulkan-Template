#include "Application.h"

Application::Application(const ApplicationInfo& Info)
    : m_Info(Info),
      m_Fullscreen(Info.Fullscreen),
      m_WindowHandle(nullptr),
      m_ElapsedTime(0.0f),
      m_FrameCount(0)
{
    glfwSetErrorCallback(
        [](int ErrorCode, const char* Description) { DEBUG_ERROR("%s", Description); });

    CHECK(glfwInit(), "Failed to initialize window.");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void Application::Init()
{
    InitWindow();
};

void Application::InitWindow()
{
    m_WindowHandle = glfwCreateWindow(
        m_Info.WindowWidth, m_Info.WindowHeight, m_Info.WindowTitle.c_str(), nullptr, nullptr);

    CHECK(m_WindowHandle != nullptr, "Failed to create window.");

    SetupWindowEvents();

    if (m_Fullscreen)
    {
        InternalSetFullscreen(true);
    }

    glfwFocusWindow(m_WindowHandle);
}

void Application::SetupWindowEvents()
{
    glfwSetWindowUserPointer(m_WindowHandle, this);

    glfwSetWindowCloseCallback(m_WindowHandle,
        [](GLFWwindow* Window)
        {
            auto App = static_cast<Application*>(glfwGetWindowUserPointer(Window));
            App->OnWindowClose();
        });

    glfwSetFramebufferSizeCallback(m_WindowHandle,
        [](GLFWwindow* Window, int Width, int Height)
        {
            auto App = static_cast<Application*>(glfwGetWindowUserPointer(Window));
            App->OnWindowResize(Width, Height);
        });

    glfwSetKeyCallback(m_WindowHandle,
        [](GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
        {
            auto App = static_cast<Application*>(glfwGetWindowUserPointer(Window));

            if (Action == GLFW_PRESS)
            {
                if (Key == GLFW_KEY_ESCAPE)
                {
                    App->Close();
                }
                else if (Key == GLFW_KEY_F11)
                {
                    App->SetFullscreen(!App->IsFullscreen());
                }

                App->OnKeyDown(Key);
            }
            else if (Action == GLFW_RELEASE)
            {
                App->OnKeyUp(Key);
            }
        });

    glfwSetMouseButtonCallback(m_WindowHandle,
        [](GLFWwindow* Window, int Button, int Action, int Mods)
        {
            auto App = static_cast<Application*>(glfwGetWindowUserPointer(Window));

            if (Action == GLFW_PRESS)
            {
                App->OnMouseDown(Button);
            }
            else if (Action == GLFW_RELEASE)
            {
                App->OnMouseUp(Button);
            }
        });

    glfwSetCursorPosCallback(m_WindowHandle,
        [](GLFWwindow* Window, double MouseX, double MouseY)
        {
            auto App = static_cast<Application*>(glfwGetWindowUserPointer(Window));
            App->OnMouseMove(static_cast<float>(MouseX), static_cast<float>(MouseY));
        });

    glfwSetScrollCallback(m_WindowHandle,
        [](GLFWwindow* Window, double OffsetX, double OffsetY)
        {
            auto App = static_cast<Application*>(glfwGetWindowUserPointer(Window));
            App->OnMouseWheel(static_cast<float>(OffsetX), static_cast<float>(OffsetY));
        });
}

void Application::Run()
{
    DEBUG_ASSERT(m_WindowHandle != nullptr);

    OnInit();

    m_Timer.Reset();

    while (!glfwWindowShouldClose(m_WindowHandle))
    {
        glfwPollEvents();

        m_Timer.Tick();
        CalculateFrameStats();

        OnUpdate(m_Timer.GetDeltaTime());
    }

    OnDestroy();
}

void Application::Close()
{
    glfwSetWindowShouldClose(m_WindowHandle, GLFW_TRUE);
}

void Application::Destroy()
{
    if (m_WindowHandle != nullptr)
    {
        glfwDestroyWindow(m_WindowHandle);
        m_WindowHandle = nullptr;
    }
};

void Application::CalculateFrameStats()
{
    m_FrameCount++;
    m_ElapsedTime += m_Timer.GetDeltaTime();

    if (m_ElapsedTime >= 1000.0f)
    {
        float FrameTime = 1000.0f / m_FrameCount;

        std::string WindowTitle = Utility::Format("%s | Rate: %u fps, Time: %.2f ms",
            m_Info.WindowTitle.c_str(), m_FrameCount, FrameTime);

        glfwSetWindowTitle(m_WindowHandle, WindowTitle.c_str());

        m_FrameCount = 0;
        m_ElapsedTime = 0;
    }
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

void Application::InternalSetFullscreen(bool Fullscreen)
{
    DEBUG_ASSERT(m_WindowHandle);

    glfwSetWindowAttrib(m_WindowHandle, GLFW_DECORATED, Fullscreen ? GLFW_FALSE : GLFW_TRUE);

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
        int PositionX = (Mode->width - m_Info.WindowWidth) / 2;
        int PositionY = (Mode->height - m_Info.WindowHeight) / 2;
        glfwSetWindowSize(m_WindowHandle, m_Info.WindowWidth, m_Info.WindowHeight);
        glfwSetWindowPos(m_WindowHandle, PositionX, PositionY);
    }
}

void Application::SetFullscreen(bool Fullscreen)
{
    Sleep(300);

    m_Fullscreen = Fullscreen;

    InternalSetFullscreen(m_Fullscreen);
}
