#pragma once
#include "pch.h"

struct MousePosition
{
    float X;
    float Y;
};

class Input
{
public:
    static void SetMode(int Mode, int Value);
    static bool IsKeyDown(int Key);
    static bool IsKeyUp(int Key);
    static bool IsMouseDown(int MouseButton);
    static bool IsMouseUp(int MouseButton);
    static MousePosition GetMousePosition();
};
