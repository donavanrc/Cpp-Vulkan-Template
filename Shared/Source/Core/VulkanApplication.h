#pragma once
#include "Application.h"
#include "pch.h"

#define NUM_BACKBUFFERS 3

struct VulkanQueue
{
    uint32_t FamilyIndex;
    VkQueue Handle = VK_NULL_HANDLE;
};

struct VulkanBackBuffer
{
    VkFormat Format;
    uint32_t Width;
    uint32_t Height;
    VkImageView ImageView = VK_NULL_HANDLE;
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
    void DestroyInstance();
    void CreateDebugMessenger();
    void DestroyDebugMessenger();
    void SelectPhysicalDevice();
    void CreateDevice();
    void DeviceWaitIdle();
    void DestroyDevice();
    VulkanQueue CreateQueue(uint32_t FamilyIndex);
    void CreateSurface();
    void DestroySurface();
    void SetupPresentQueue();
    VkPresentModeKHR GetPresentMode();
    VkSurfaceFormatKHR GetSurfaceFormat(VkFormat Format);
    VkCompositeAlphaFlagBitsKHR GetCompositeAlpha(VkSurfaceCapabilitiesKHR SurfaceCapabilities);
    VkSurfaceTransformFlagBitsKHR GetPreTransform(VkSurfaceCapabilitiesKHR SurfaceCapabilities);
    void CreateSwapChain(VkFormat Format, uint32_t Width, uint32_t Height, uint32_t NumBackBuffers);
    void DestroySwapChain();
    void CreateBackBuffers(VkFormat Format, uint32_t Width, uint32_t Height);
    void DestroyBackBuffers();
    VkImageView CreateImageView(VkImageViewType Type, VkFormat Format, VkImage Image);
    void CreateRenderPass(VulkanBackBuffer& ColorBuffer);
    void DestroyRenderPass();
    VkFramebuffer CreateFrameBuffer(VulkanBackBuffer& BackBuffer);
    void CreateFrameBuffers();
    void DestroyFrameBuffer(VkFramebuffer& FrameBuffer);
    void CreateCommandPool();
    void DestroyCommandPool();
    VkCommandBuffer CreateCommandBuffer();
    void DestroyCommandBuffer();
    VkFence CreateFence(bool Signaled = false);
    void DestroyFence(VkFence& Fence);
    VkSemaphore CreateSemaphore();
    void DestroySemaphore(VkSemaphore& Semaphore);
    void CreateSyncObjects();
    void DestroySyncObjects();

    bool AcquireImageIndex(uint32_t* OutImageIndex);
    bool Present();
    VkCommandBuffer BeginCommandBuffer();
    void EndCommandBuffer(VkCommandBuffer& CommandBuffer);
    void Submit(VkCommandBuffer& CommandBuffer);
    void BeginRenderPass(VkCommandBuffer& CommandBuffer);
    void EndRenderPass(VkCommandBuffer& CommandBuffer);

    static VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT MessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* CallbackDataPtr, void* UserDataPtr);

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VulkanQueue m_GraphicsQueue;
    VkSurfaceKHR m_Surface;
    VulkanQueue m_PresentQueue;
    VkSwapchainKHR m_SwapChain;
    std::vector<VulkanBackBuffer> m_BackBuffers;
    VkRenderPass m_RenderPass;
    std::vector<VkFramebuffer> m_FrameBuffers;
    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    std::vector<VkFence> m_AcquiredImageFences;
    std::vector<VkSemaphore> m_AcquiredImageSemaphores;
    std::vector<VkFence> m_RenderingDoneFences;
    std::vector<VkSemaphore> m_RenderingDoneSemaphores;
    uint32_t m_CurrentFrameIndex;
    uint32_t m_ImageSemaphoreIndex;
    uint32_t m_CommandBufferIndex;
};