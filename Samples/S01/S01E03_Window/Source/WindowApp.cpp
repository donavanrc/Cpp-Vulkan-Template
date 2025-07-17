#include "WindowApp.h"
#include "Core/Entrypoint.h"

WindowApp::WindowApp() : Application({"S01E03 Window", 800, 600}) {}

void WindowApp::OnInit()
{
    DEBUG_DISPLAY("OnInit()");
}

void WindowApp::OnDestroy()
{
    DEBUG_DISPLAY("OnDestroy()");
}

void WindowApp::OnUpdate(float DeltaTime)
{
    DEBUG_DISPLAY("OnUpdate(%.2f)", DeltaTime);
    Sleep(100);
}

void WindowApp::OnKeyDown(int KeyCode)
{
    DEBUG_DISPLAY("OnKeyDown(%i)", KeyCode);
}

void WindowApp::OnKeyUp(int KeyCode)
{
    DEBUG_DISPLAY("OnKeyUp(%i)", KeyCode);
}

void WindowApp::OnMouseDown(int Button)
{
    DEBUG_DISPLAY("OnMouseDown(%i)", Button);
}

void WindowApp::OnMouseUp(int Button)
{
    DEBUG_DISPLAY("OnMouseUp(%i)", Button);
}

void WindowApp::OnMouseMove(float MouseX, float MouseY)
{
    DEBUG_DISPLAY("OnMouseMove(%.2f, %.2f)", MouseX, MouseY);
}

void WindowApp::OnMouseWheel(float OffsetX, float OffsetY)
{
    DEBUG_DISPLAY("OnMouseWheel(%.2f, %.2f)", OffsetX, OffsetY);
}

void WindowApp::OnWindowClose()
{
    DEBUG_DISPLAY("OnWindowClose()");
}

void WindowApp::OnWindowResize(uint32_t Width, uint32_t Height)
{
    DEBUG_DISPLAY("OnWindowResize(%u, %u)", Width, Height);
}

START_APPLICATION(WindowApp);