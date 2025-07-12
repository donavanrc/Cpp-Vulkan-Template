#include "Core/Input.h"
#include "Platform/Window.h"
#include "pch.h"

int main()
{
    Platform::WindowDesc WindowDesc = {};
    WindowDesc.Title = "S01EP02 | Window";
    WindowDesc.Width = 800;
    WindowDesc.Height = 600;
    WindowDesc.Fullscreen = true;

    Platform::Window Window(WindowDesc);
    Window.Initialize();

    while (!Window.WantsExit())
    {
        Window.ProcessEvents();

        if (Input::IsKeyDown(GLFW_KEY_ESCAPE))
        {
            break;
        }

        if (Input::IsKeyDown(GLFW_KEY_F11))
        {
            Window.SetFullscreen(!Window.IsFullscreen());
        }
    }

    Window.Destroy();

    return 0;
}