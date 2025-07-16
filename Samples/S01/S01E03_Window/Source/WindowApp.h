#pragma once
#include "Core/Application.h"
#include "pch.h"

class WindowApp : public Application
{
public:
    void Initialize() override;
    void Destroy() override;
    void Tick(float DeltaTime) override;
};