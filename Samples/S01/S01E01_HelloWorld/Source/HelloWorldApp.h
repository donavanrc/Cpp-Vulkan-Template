#pragma once
#include "pch.h"
#include "Core/Entrypoint.h"

class HelloWorldApp : public IApplication
{
public:
    void Initialize() override;
    void Run() override;
    void Destroy() override;
};