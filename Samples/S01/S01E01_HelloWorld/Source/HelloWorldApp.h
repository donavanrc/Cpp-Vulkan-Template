#pragma once
#include "pch.h"
#include "Core/Entrypoint.h"

class HelloWorldApp : public Core::Application
{
public:
    void Initialize() override;
    void Run() override;
    void Destroy() override;
};