#pragma once
#include "IApplication.h"
#include "Timer.h"
#include "pch.h"

struct ApplicationInfo
{
    std::string WindowTitle;
    uint32_t WindowWidth;
    uint32_t WindowHeight;
    bool Fullscreen = false;
    uint32_t ApiVersion = VK_API_VERSION_1_2;
};

class Application : public IApplication
{
public:
    Application(const ApplicationInfo& Info);
    ~Application() = default;
    void Close();
    virtual void Init() override;
    virtual void Destroy() override;
    virtual void Run() override;

protected:
    void InitWindow();
    void CalculateFrameStats();
    void SetupWindowEvents();
    void InternalSetFullscreen(bool Fullscreen);
    bool IsFullscreen() { return m_Fullscreen; }
    void SetFullscreen(bool Fullscreen);

    void Sleep(uint32_t Milliseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(Milliseconds));
    };

    virtual void OnInit() = 0;
    virtual void OnDestroy() = 0;
    virtual void OnUpdate(float DeltaTime) = 0;

    virtual void OnKeyDown(int KeyCode) {};
    virtual void OnKeyUp(int KeyCode) {};
    virtual void OnMouseDown(int Button) {};
    virtual void OnMouseUp(int Button) {};
    virtual void OnMouseMove(float MouseX, float MouseY) {};
    virtual void OnMouseWheel(float OffsetX, float OffsetY) {};
    virtual void OnWindowClose() {};
    virtual void OnWindowResize(uint32_t Width, uint32_t Height) {};

    ApplicationInfo m_Info;
    GLFWwindow* m_WindowHandle;
    Timer m_Timer;
    float m_ElapsedTime;
    uint32_t m_FrameCount;
    bool m_Fullscreen;
};