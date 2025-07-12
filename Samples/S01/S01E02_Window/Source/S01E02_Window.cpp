#include "pch.h"
#include "Platform/Window.h"

int main()
{
    Platform::WindowDesc WindowDesc = {};
    WindowDesc.Title = "S01EP02 | Window";
    WindowDesc.Width = 800;
    WindowDesc.Height = 600;

    Platform::Window Window(WindowDesc);
    Window.Initialize();

    while (!Window.WantsExit())
    {
        Window.ProcessEvents();
    }

    Window.Destroy();

    return 0;
}