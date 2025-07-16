#include "Application.h"

Application::Application(
    const std::string& WindowTitle, uint32_t WindowWidth, uint32_t WindowHeight)
    : m_WindowTitle(WindowTitle),
      m_WindowWidth(WindowWidth),
      m_WindowHeight(WindowHeight),
      m_WindowHandle(nullptr),
      m_Running(false),
      m_ElapsedTime(0.0f),
      m_FrameCount(0)
{
    glfwSetErrorCallback(
        [](int ErrorCode, const char* Description) { DEBUG_ERROR("%s", Description); });

    CHECK(glfwInit(), "Failed to initialize window.");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void Application::Initialize()
{
    InitializeWindow();
    InitializeGraphics();
    RunMainLoop();
};

void Application::InitializeWindow()
{
    m_WindowHandle =
        glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowTitle.c_str(), nullptr, nullptr);

    CHECK(m_WindowHandle != nullptr, "Failed to create window.");

    SetupWindowEvents();
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

void Application::RunMainLoop()
{
    DEBUG_ASSERT(m_WindowHandle != nullptr);
    DEBUG_ASSERT(!m_Running);

    m_Running = true;

    m_Timer.Reset();

    while (m_Running)
    {
        glfwPollEvents();

        m_Timer.Tick();
        CalculateFrameStats();

        Tick(m_Timer.GetDeltaTime());
    }
}

void Application::Destroy()
{
    if (m_WindowHandle != nullptr)
    {
        glfwDestroyWindow(m_WindowHandle);
        m_WindowHandle = false;
    }
};

void Application::CalculateFrameStats()
{
    m_FrameCount++;
    m_ElapsedTime += m_Timer.GetDeltaTime();

    if (m_ElapsedTime >= 1000.0f)
    {
        float FrameTime = 1000.0f / m_FrameCount;

        std::string WindowTitle = Utility::Format(
            "%s | Rate: %u fps, Time: %.2f ms", m_WindowTitle.c_str(), m_FrameCount, FrameTime);

        SetWindowTitle(WindowTitle.c_str());

        m_FrameCount = 0;
        m_ElapsedTime = 0;
    }
}

void Application::SetWindowTitle(const std::string& WindowTitle)
{
    DEBUG_ASSERT(m_WindowHandle != nullptr);

    m_WindowTitle = WindowTitle;
    glfwSetWindowTitle(m_WindowHandle, m_WindowTitle.c_str());
}