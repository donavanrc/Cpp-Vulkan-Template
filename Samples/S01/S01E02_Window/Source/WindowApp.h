#pragma once
#include "pch.h"
#include "Core/Entrypoint.h"
#include "Core/Input.h"
#include "Platform/Window.h"

class WindowApp : public Core::Application
{
public:
    void Initialize() override;
    void Destroy() override;
    void Run() override;

    void OnWindowClose();
    void OnWindowResize(uint32_t Width, uint32_t Height) override;

private:
    bool m_Running = false;
    std::unique_ptr<Platform::Window> m_Window;
};