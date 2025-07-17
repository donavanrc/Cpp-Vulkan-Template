#pragma once
#include "pch.h"
#include "Core/Entrypoint.h"

class HelloWorldApp : public IApplication
{
public:
    void Init() override;
    void Destroy() override;
    void Run() override;
};