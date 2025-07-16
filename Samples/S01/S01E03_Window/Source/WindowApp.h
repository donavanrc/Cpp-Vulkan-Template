#pragma once
#include "Core/Application.h"
#include "pch.h"

class WindowApp : public Application
{
public:
    WindowApp();
    ~WindowApp() = default;

protected:
    void OnInit() override;
    void OnDestroy() override;
    void OnUpdate(float DeltaTime) override;
    void OnKeyDown(int KeyCode) override;
    void OnKeyUp(int KeyCode) override;
    void OnMouseDown(int Button) override;
    void OnMouseUp(int Button) override;
    void OnMouseMove(float MouseX, float MouseY) override;
    void OnMouseWheel(float OffsetX, float OffsetY) override;
    void OnWindowClose() override;
    void OnWindowResize(uint32_t Width, uint32_t Height) override;
};