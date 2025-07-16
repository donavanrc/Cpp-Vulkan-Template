#pragma once
#include "IApplication.h"
#include "Timer.h"
#include "pch.h"

class Application : public IApplication
{
public:
    Application(const std::string& WindowTitle, uint32_t WindowWidth, uint32_t WindowHeight);
    ~Application() = default;
    virtual void Initialize() override;
    virtual void Destroy() override;

private:
    void InitializeWindow();
    void CalculateFrameStats();
    void SetupWindowEvents();
    void RunMainLoop();

protected:
    virtual void InitializeGraphics() = 0;
    virtual void Tick(float DeltaTime) = 0;
    void SetWindowTitle(const std::string& Title);

    virtual void OnKeyDown(int KeyCode) {};
    virtual void OnKeyUp(int KeyCode) {};
    virtual void OnMouseDown(int Button) {};
    virtual void OnMouseUp(int Button) {};
    virtual void OnMouseMove(float MouseX, float MouseY) {};
    virtual void OnMouseWheel(float OffsetX, float OffsetY) {};
    virtual void OnWindowClose() {};
    virtual void OnWindowResize(uint32_t Width, uint32_t Height) {};

    bool m_Running;
    std::string m_WindowTitle;
    uint32_t m_WindowWidth;
    uint32_t m_WindowHeight;
    GLFWwindow* m_WindowHandle;
    Timer m_Timer;
    float m_ElapsedTime;
    uint32_t m_FrameCount;
};