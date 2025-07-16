#pragma once
#include "Application.h"
#include "pch.h"

class VulkanApplication : public Application
{
public:
    virtual void Initialize() override;
    virtual void Destroy() override;
};