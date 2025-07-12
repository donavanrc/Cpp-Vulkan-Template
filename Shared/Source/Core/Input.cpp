#include "Input.h"
#include "Platform/Window.h"

bool Input::IsKeyDown(int Key)
{
    auto* Window = Platform::Window::GetInstance()->GetWindowHandle();
    auto State = glfwGetKey(Window, static_cast<int>(Key));
    return State == GLFW_PRESS;
}

bool Input::IsKeyUp(int Key)
{
    auto* Window = Platform::Window::GetInstance()->GetWindowHandle();
    auto State = glfwGetKey(Window, static_cast<int>(Key));
    return State == GLFW_RELEASE;
}

bool Input::IsMouseDown(int MouseButton)
{
    auto* Window = Platform::Window::GetInstance()->GetWindowHandle();
    auto State = glfwGetMouseButton(Window, static_cast<int>(MouseButton));
    return State == GLFW_PRESS;
}

bool Input::IsMouseUp(int MouseButton)
{
    auto* Window = Platform::Window::GetInstance()->GetWindowHandle();
    auto State = glfwGetMouseButton(Window, static_cast<int>(MouseButton));
    return State == GLFW_RELEASE;
}

MousePosition Input::GetMousePosition()
{
    auto* Window = Platform::Window::GetInstance()->GetWindowHandle();
    double MouseX, MouseY;
    glfwGetCursorPos(Window, &MouseX, &MouseY);
    return {static_cast<float>(MouseX), static_cast<float>(MouseY)};
}
