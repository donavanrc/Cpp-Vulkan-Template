#pragma once
#include "IApplication.h"
#include "Timer.h"
#include "pch.h"

class Application : public IApplication
{
public:
    Application(const std::string& WindowTitle, uint32_t WindowWidth, uint32_t WindowHeight,
        bool Resizable = false);
    ~Application() = default;
    void Close();
    virtual void Init() override;
    virtual void Destroy() override;

private:
    void InitWindow();
    void CalculateFrameStats();
    void SetupWindowEvents();
    void RunLoop();

    void InternalSetFullscreen(bool Fullscreen);

protected:
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

    std::string m_WindowTitle;
    uint32_t m_WindowWidth;
    uint32_t m_WindowHeight;
    GLFWwindow* m_WindowHandle;
    bool m_Fullscreen;
    Timer m_Timer;
    float m_ElapsedTime;
    uint32_t m_FrameCount;
};