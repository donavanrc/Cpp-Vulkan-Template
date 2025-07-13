#pragma once
#include "pch.h"
#include "Core/Entrypoint.h"
#include "Core/Input.h"
#include "Core/Timer.h"
#include "Platform/Window.h"

class WindowApp : public Core::Application
{
public:
    void Initialize() override;
    void Destroy() override;
    void Run() override;
    void CalculateFrameStats();

    void OnWindowClose();
    void OnWindowResize(uint32_t Width, uint32_t Height) override;

private:
    bool m_Running = false;
    Platform::WindowDesc m_WindowDesc;
    std::unique_ptr<Platform::Window> m_Window;
    Core::Timer m_Timer;
    float m_ElapsedTime = 0.0f;
    uint32_t m_FrameCount = 0;
};