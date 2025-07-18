#pragma once
#include "Application.h"
#include "pch.h"

struct VulkanQueue
{
    VkQueue Queue;
    uint32_t FamilyIndex;
};

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
    void CreateDebugMessenger();
    void DestroyDebugMessenger();
    void SelectPhysicalDevice();
    void CreateDevice();

    static VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT MessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* CallbackDataPtr, void* UserDataPtr);

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VulkanQueue m_GraphicsQueue;
    VulkanQueue m_PresentQueue;
};