#pragma once
#include "Core/VulkanApplication.h"
#include "pch.h"

class HelloVulkanApp : public VulkanApplication
{
public:
    HelloVulkanApp();
    ~HelloVulkanApp() = default;

protected:
    void OnInit();
    void OnDestroy();
    void OnUpdate(float DeltaTime);
};