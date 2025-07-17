#pragma once
#include "Application.h"
#include "pch.h"

class VulkanApplication : public Application
{
public:
    VulkanApplication(const ApplicationInfo& Info);
    ~VulkanApplication() = default;
    virtual void Init() override;
    virtual void Destroy() override;

protected:
    void InitGraphics();
    void CreateInstance();

    VkInstance m_Instance;
};