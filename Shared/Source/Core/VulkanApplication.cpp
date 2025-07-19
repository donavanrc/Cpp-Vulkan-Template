#include "VulkanApplication.h"

#define VULKAN_RESULT(Result) CHECK(Result == VK_SUCCESS, "Vulkan Error (%d): %s", Result, #Result)

VulkanApplication::VulkanApplication(const ApplicationInfo& Info)
    : Application(Info),
      m_Instance(VK_NULL_HANDLE),
      m_DebugMessenger(VK_NULL_HANDLE),
      m_Device(VK_NULL_HANDLE),
      m_Surface(VK_NULL_HANDLE),
      m_SwapChain(VK_NULL_HANDLE),
      m_RenderPass(VK_NULL_HANDLE),
      m_CommandPool(VK_NULL_HANDLE),
      m_CurrentFrameIndex(-1),
      m_ImageSemaphoreIndex(-1),
      m_CommandBufferIndex(-1)
{
}

void VulkanApplication::Init()
{
    Application::Init();

    InitGraphics();
};

void VulkanApplication::Destroy()
{
    DeviceWaitIdle();

    DestroySyncObjects();
    DestroyCommandPool();

    for (auto& FrameBuffer : m_FrameBuffers)
    {
        DestroyFrameBuffer(FrameBuffer);
    }

    DestroyRenderPass();
    DestroyBackBuffers();
    DestroySwapChain();
    DestroySurface();
    DestroyDevice();
    DestroyDebugMessenger();
    DestroyInstance();

    Application::Destroy();
};

void VulkanApplication::InitGraphics()
{
    CreateInstance();
    CreateDebugMessenger();
    SelectPhysicalDevice();
    CreateDevice();
    CreateSurface();
    SetupPresentQueue();
    CreateSwapChain(
        VK_FORMAT_B8G8R8A8_SRGB, m_Info.WindowWidth, m_Info.WindowHeight, NUM_BACKBUFFERS);
    CreateBackBuffers(VK_FORMAT_B8G8R8A8_SRGB, m_Info.WindowWidth, m_Info.WindowHeight);
    CreateRenderPass(m_BackBuffers[0]);

    m_FrameBuffers.resize(NUM_BACKBUFFERS);
    for (uint32_t Index = 0; Index < m_FrameBuffers.size(); ++Index)
    {
        m_FrameBuffers[Index] = CreateFrameBuffer(m_BackBuffers[Index]);
    }

    CreateCommandPool();
    CreateSyncObjects();
}

inline std::vector<const char*> GetValidationLayers()
{
    return {"VK_LAYER_KHRONOS_validation"};
}

inline std::vector<const char*> GetInstanceExtensions()
{
    std::vector<const char*> InstanceExtensions = {"VK_EXT_debug_utils"};

    uint32_t ExtensionCount = 0;
    auto Extensions = glfwGetRequiredInstanceExtensions(&ExtensionCount);
    for (uint32_t Index = 0; Index < ExtensionCount; ++Index)
    {
        InstanceExtensions.push_back(Extensions[Index]);
    }
    return InstanceExtensions;
}

void VulkanApplication::CreateInstance()
{
    uint32_t ApiVersion = m_Info.ApiVersion;

    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.apiVersion = ApiVersion;
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pApplicationName = m_Info.WindowTitle.c_str();
    AppInfo.pEngineName = "Vulkan Engine";

    VkInstanceCreateInfo InstanceInfo = {};
    InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceInfo.pApplicationInfo = &AppInfo;

    auto Extensions = GetInstanceExtensions();
    InstanceInfo.enabledExtensionCount = Extensions.size();
    InstanceInfo.ppEnabledExtensionNames = Extensions.size() > 0 ? Extensions.data() : nullptr;

    auto Layers = GetValidationLayers();
    InstanceInfo.enabledLayerCount = Layers.size();
    InstanceInfo.ppEnabledLayerNames = Layers.size() > 0 ? Layers.data() : nullptr;

    VULKAN_RESULT(vkCreateInstance(&InstanceInfo, nullptr, &m_Instance));

    DEBUG_DISPLAY("Target API version: %d.%d.%d", VK_API_VERSION_MAJOR(ApiVersion),
        VK_API_VERSION_MINOR(ApiVersion), VK_API_VERSION_PATCH(ApiVersion));
}

void VulkanApplication::DestroyInstance()
{
    if (m_Instance)
    {
        vkDestroyInstance(m_Instance, nullptr);
        m_Instance = VK_NULL_HANDLE;
    }
}

VkBool32 VKAPI_CALL VulkanApplication::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT MessageType,
    const VkDebugUtilsMessengerCallbackDataEXT* CallbackDataPtr, void* UserDataPtr)
{
    if (MessageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        DEBUG_ERROR("Debug callback: %s", CallbackDataPtr->pMessage);
    }
    return VK_FALSE;
}

void VulkanApplication::CreateDebugMessenger()
{
    DEBUG_ASSERT(m_Instance != VK_NULL_HANDLE);

    VkDebugUtilsMessengerCreateInfoEXT DebugInfo = {};
    DebugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    DebugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    DebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    DebugInfo.pfnUserCallback = DebugCallback;

    auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        m_Instance, "vkCreateDebugUtilsMessengerEXT");
    if (vkCreateDebugUtilsMessengerEXT == nullptr)
    {
        VULKAN_RESULT(VK_ERROR_EXTENSION_NOT_PRESENT);
    }
    VULKAN_RESULT(
        vkCreateDebugUtilsMessengerEXT(m_Instance, &DebugInfo, nullptr, &m_DebugMessenger));
}

void VulkanApplication::DestroyDebugMessenger()
{
    if (m_DebugMessenger)
    {
        auto vkDestroyDebugUtilsMessengerEXT =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                m_Instance, "vkDestroyDebugUtilsMessengerEXT");

        vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
        m_DebugMessenger = VK_NULL_HANDLE;
    }
}

inline std::string GetDeviceTypeDescription(VkPhysicalDeviceType DeviceType)
{
    switch (DeviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "Discrete";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "Integrated";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "Cpu";
    }

    DEBUG_ASSERT(false, "Invalid device type");
    return "";
}

void VulkanApplication::SelectPhysicalDevice()
{
    DEBUG_ASSERT(m_Instance != VK_NULL_HANDLE);

    uint32_t PhysicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &PhysicalDeviceCount, nullptr);

    CHECK(PhysicalDeviceCount > 0, "No devices found");

    std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &PhysicalDeviceCount, PhysicalDevices.data());

    struct
    {
        VkPhysicalDevice Device = VK_NULL_HANDLE;
        VkPhysicalDeviceType DeviceType;
    } SelectedPhysicalDevice;

    for (const auto& CurrentPhysicalDevice : PhysicalDevices)
    {
        VkPhysicalDeviceProperties Properties;
        vkGetPhysicalDeviceProperties(CurrentPhysicalDevice, &Properties);

        if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            SelectedPhysicalDevice.Device = CurrentPhysicalDevice;
            SelectedPhysicalDevice.DeviceType = Properties.deviceType;
            break;
        }

        if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            SelectedPhysicalDevice.Device = CurrentPhysicalDevice;
            SelectedPhysicalDevice.DeviceType = Properties.deviceType;
        }

        if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU &&
            SelectedPhysicalDevice.DeviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            SelectedPhysicalDevice.Device = CurrentPhysicalDevice;
            SelectedPhysicalDevice.DeviceType = Properties.deviceType;
        }
    }

    CHECK(SelectedPhysicalDevice.Device != VK_NULL_HANDLE, "No physical device found");

    m_PhysicalDevice = SelectedPhysicalDevice.Device;

    VkPhysicalDeviceProperties Properties;
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &Properties);

    DEBUG_DISPLAY("Selected device: %s", Properties.deviceName);
    DEBUG_DISPLAY("Device type: %s", GetDeviceTypeDescription(Properties.deviceType).c_str());
    DEBUG_DISPLAY("Device API version: %d.%d.%d", VK_API_VERSION_MAJOR(Properties.apiVersion),
        VK_API_VERSION_MINOR(Properties.apiVersion), VK_API_VERSION_PATCH(Properties.apiVersion));
}

inline std::vector<const char*> GetDeviceExtensions()
{
    return {"VK_KHR_swapchain"};
}

inline std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(
    VkPhysicalDevice PhysicalDevice)
{
    DEBUG_ASSERT(PhysicalDevice != VK_NULL_HANDLE);

    uint32_t QueueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyPropertiesCount, nullptr);

    CHECK(QueueFamilyPropertiesCount, "No suitable queue family found");

    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        PhysicalDevice, &QueueFamilyPropertiesCount, QueueFamilies.data());

    DEBUG_DISPLAY("Queues found: %d", QueueFamilies.size());
    return QueueFamilies;
}

inline std::string GetQueueFamilyDescription(VkQueueFamilyProperties Properties)
{
    std::string Output;

    if (Properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
        Output += "Graphics ";
    }

    if (Properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
        Output += "Compute ";
    }

    if (Properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
    {
        Output += "Transfer ";
    }

    return Output.substr(0, Output.length() - 1);
}

void VulkanApplication::CreateDevice()
{
    DEBUG_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE);

    std::vector<VkQueueFamilyProperties> QueueFamilies = GetQueueFamilyProperties(m_PhysicalDevice);

    std::vector<float> QueuePriorities;
    std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;

    uint32_t GraphicsQueueFamilyIndex = UINT32_MAX;

    for (uint32_t FamilyIndex = 0; FamilyIndex < QueueFamilies.size(); ++FamilyIndex)
    {
        auto CurrentFamilyQueue = QueueFamilies[FamilyIndex];
        bool ValidQueue = false;

        if ((CurrentFamilyQueue.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
        {
            if (GraphicsQueueFamilyIndex == UINT32_MAX)
            {
                GraphicsQueueFamilyIndex = FamilyIndex;
                ValidQueue = true;
            }
        }

        if (!ValidQueue)
        {
            DEBUG_WARNING("Skipping unnecessary family queue (index=%d, count=%d): %s", FamilyIndex,
                CurrentFamilyQueue.queueCount,
                GetQueueFamilyDescription(CurrentFamilyQueue).c_str());
            continue;
        }

        QueuePriorities.resize(CurrentFamilyQueue.queueCount);
        std::fill(QueuePriorities.begin(), QueuePriorities.end(), 1.0f);

        VkDeviceQueueCreateInfo QueueCreateInfo = {};
        QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfo.pQueuePriorities = QueuePriorities.data();
        QueueCreateInfo.queueCount = CurrentFamilyQueue.queueCount;
        QueueCreateInfo.queueFamilyIndex = FamilyIndex;

        DEBUG_DISPLAY("Family queue (index=%d, count=%d): %s", FamilyIndex,
            CurrentFamilyQueue.queueCount, GetQueueFamilyDescription(CurrentFamilyQueue).c_str());

        QueueCreateInfos.push_back(QueueCreateInfo);
    }

    VkDeviceCreateInfo DeviceInfo = {};
    DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    auto Extensions = GetDeviceExtensions();
    DeviceInfo.enabledExtensionCount = Extensions.size();
    DeviceInfo.ppEnabledExtensionNames = Extensions.data();
    auto Layers = GetValidationLayers();
    DeviceInfo.enabledLayerCount = Layers.size();
    DeviceInfo.ppEnabledLayerNames = Layers.data();
    DeviceInfo.pEnabledFeatures = nullptr;
    DeviceInfo.queueCreateInfoCount = QueueCreateInfos.size();
    DeviceInfo.pQueueCreateInfos = QueueCreateInfos.data();

    VULKAN_RESULT(vkCreateDevice(m_PhysicalDevice, &DeviceInfo, nullptr, &m_Device));

    if (GraphicsQueueFamilyIndex != UINT32_MAX)
    {
        m_GraphicsQueue = CreateQueue(GraphicsQueueFamilyIndex);
    }
}

void VulkanApplication::DeviceWaitIdle()
{
    DEBUG_ASSERT(m_Device);

    vkDeviceWaitIdle(m_Device);
}

void VulkanApplication::DestroyDevice()
{
    if (m_Device)
    {
        vkDestroyDevice(m_Device, nullptr);
        m_Device = VK_NULL_HANDLE;
    }
}

VulkanQueue VulkanApplication::CreateQueue(uint32_t FamilyIndex)
{
    VulkanQueue Queue{.FamilyIndex = FamilyIndex};
    vkGetDeviceQueue(m_Device, Queue.FamilyIndex, 0, &Queue.Handle);
    return Queue;
}

void VulkanApplication::CreateSurface()
{
    DEBUG_ASSERT(m_WindowHandle);
    DEBUG_ASSERT(m_Instance);

    VULKAN_RESULT(glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface));
}

void VulkanApplication::DestroySurface()
{
    if (m_Surface)
    {
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        m_Surface = VK_NULL_HANDLE;
    }
}

void VulkanApplication::SetupPresentQueue()
{
    if (m_PresentQueue.Handle == VK_NULL_HANDLE)
    {
        VkBool32 PresentSupported;
        VULKAN_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(
            m_PhysicalDevice, m_GraphicsQueue.FamilyIndex, m_Surface, &PresentSupported));

        if (PresentSupported)
        {
            m_PresentQueue = m_GraphicsQueue;
        }

        CHECK(m_PresentQueue.Handle, "No compatible present queue found");
    }
}

VkPresentModeKHR VulkanApplication::GetPresentMode()
{
    uint32_t PresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        m_PhysicalDevice, m_Surface, &PresentModeCount, nullptr);

    CHECK(PresentModeCount, "No present modes found");

    std::vector<VkPresentModeKHR> PresentModes(PresentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        m_PhysicalDevice, m_Surface, &PresentModeCount, PresentModes.data());

    VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;

    for (auto& CurrentPresentMode : PresentModes)
    {
        if (CurrentPresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            PresentMode = CurrentPresentMode;
            break;
        }

        if (CurrentPresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            PresentMode = CurrentPresentMode;
        }
    }

    return PresentMode;
}

VkSurfaceFormatKHR VulkanApplication::GetSurfaceFormat(VkFormat Format)
{
    uint32_t SurfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &SurfaceFormatCount, nullptr);

    CHECK(SurfaceFormatCount, "No surface format found");

    std::vector<VkSurfaceFormatKHR> SurfaceFormats(SurfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        m_PhysicalDevice, m_Surface, &SurfaceFormatCount, SurfaceFormats.data());

    VkSurfaceFormatKHR SurfaceFormat = SurfaceFormats[0];

    for (auto& CurrentSurfaceFormat : SurfaceFormats)
    {
        if (CurrentSurfaceFormat.format == Format &&
            CurrentSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            SurfaceFormat = CurrentSurfaceFormat;
        }
    }

    return SurfaceFormat;
}

VkCompositeAlphaFlagBitsKHR VulkanApplication::GetCompositeAlpha(
    VkSurfaceCapabilitiesKHR SurfaceCapabilities)
{
    VkCompositeAlphaFlagBitsKHR CompositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    if (SurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
    {
        CompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    }

    return CompositeAlpha;
}

VkSurfaceTransformFlagBitsKHR VulkanApplication::GetPreTransform(
    VkSurfaceCapabilitiesKHR SurfaceCapabilities)
{
    VkSurfaceTransformFlagBitsKHR PreTransform;
    if (SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        PreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        PreTransform = SurfaceCapabilities.currentTransform;
    }

    return PreTransform;
}

void VulkanApplication::CreateSwapChain(
    VkFormat Format, uint32_t Width, uint32_t Height, uint32_t NumBackBuffers)
{
    DEBUG_ASSERT(m_PhysicalDevice);
    DEBUG_ASSERT(m_Device);
    DEBUG_ASSERT(m_Surface);

    VkSurfaceFormatKHR SurfaceFormat = GetSurfaceFormat(Format);

    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &SurfaceCapabilities);

    VkSwapchainCreateInfoKHR SwapChainInfo = {};
    SwapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapChainInfo.minImageCount = NumBackBuffers;
    SwapChainInfo.imageExtent.width = Width;
    SwapChainInfo.imageExtent.height = Height;
    SwapChainInfo.imageColorSpace = SurfaceFormat.colorSpace;
    SwapChainInfo.imageFormat = SurfaceFormat.format;
    SwapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapChainInfo.imageArrayLayers = 1;
    SwapChainInfo.compositeAlpha = GetCompositeAlpha(SurfaceCapabilities);
    SwapChainInfo.presentMode = GetPresentMode();
    SwapChainInfo.preTransform = GetPreTransform(SurfaceCapabilities);
    SwapChainInfo.surface = m_Surface;
    SwapChainInfo.oldSwapchain = VK_NULL_HANDLE;
    SwapChainInfo.clipped = VK_TRUE;

    VULKAN_RESULT(vkCreateSwapchainKHR(m_Device, &SwapChainInfo, nullptr, &m_SwapChain));

    DEBUG_DISPLAY("SwapChain images: %d", SwapChainInfo.minImageCount);
    DEBUG_DISPLAY("SwapChain extent: (%d, %d)", SwapChainInfo.imageExtent.width,
        SwapChainInfo.imageExtent.height);
}

void VulkanApplication::DestroySwapChain()
{
    if (m_SwapChain)
    {
        vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
        m_SwapChain = VK_NULL_HANDLE;
    }
}

void VulkanApplication::CreateBackBuffers(VkFormat Format, uint32_t Width, uint32_t Height)
{
    uint32_t SwapChainImageCount = 0;
    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &SwapChainImageCount, nullptr);
    std::vector<VkImage> SwapChainImages(SwapChainImageCount);
    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &SwapChainImageCount, SwapChainImages.data());

    m_BackBuffers.resize(SwapChainImageCount);
    for (uint32_t Index = 0; Index < SwapChainImages.size(); ++Index)
    {
        m_BackBuffers[Index].Format = Format;
        m_BackBuffers[Index].Width = Width;
        m_BackBuffers[Index].Height = Height;
        m_BackBuffers[Index].ImageView =
            CreateImageView(VK_IMAGE_VIEW_TYPE_2D, Format, SwapChainImages[Index]);
    }
}

void VulkanApplication::DestroyBackBuffers()
{
    for (auto& BackBuffer : m_BackBuffers)
    {
        vkDestroyImageView(m_Device, BackBuffer.ImageView, nullptr);
        BackBuffer.ImageView = VK_NULL_HANDLE;
    }
}

VkImageView VulkanApplication::CreateImageView(VkImageViewType Type, VkFormat Format, VkImage Image)
{
    VkImageViewCreateInfo ViewInfo = {};
    ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ViewInfo.viewType = Type;
    ViewInfo.format = Format;
    ViewInfo.image = Image;
    ViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    ViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    ViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    ViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ViewInfo.subresourceRange.baseArrayLayer = 0;
    ViewInfo.subresourceRange.baseMipLevel = 0;
    ViewInfo.subresourceRange.layerCount = 1;
    ViewInfo.subresourceRange.levelCount = 1;

    VkImageView ImageView = VK_NULL_HANDLE;
    VULKAN_RESULT(vkCreateImageView(m_Device, &ViewInfo, nullptr, &ImageView));
    return ImageView;
}

void VulkanApplication::CreateRenderPass(VulkanBackBuffer& ColorBuffer)
{
    VkAttachmentDescription ColorAttachment = {};
    ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    ColorAttachment.format = ColorBuffer.Format;
    ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkAttachmentDescription> Attachments = {ColorAttachment};

    VkAttachmentReference ColorAttachmentRef = {};
    ColorAttachmentRef.attachment = 0;
    ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription Subpass = {};
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    Subpass.colorAttachmentCount = 1;
    Subpass.pColorAttachments = &ColorAttachmentRef;

    VkSubpassDependency Dependency = {};
    Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    Dependency.dstSubpass = 0;
    Dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    Dependency.srcAccessMask = 0;
    Dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    Dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo RenderPassInfo = {};
    RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassInfo.attachmentCount = Attachments.size();
    RenderPassInfo.pAttachments = Attachments.data();
    RenderPassInfo.subpassCount = 1;
    RenderPassInfo.pSubpasses = &Subpass;
    RenderPassInfo.dependencyCount = 1;
    RenderPassInfo.pDependencies = &Dependency;

    VULKAN_RESULT(vkCreateRenderPass(m_Device, &RenderPassInfo, nullptr, &m_RenderPass));
}

void VulkanApplication::DestroyRenderPass()
{
    if (m_RenderPass)
    {
        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
        m_RenderPass = VK_NULL_HANDLE;
    }
}

VkFramebuffer VulkanApplication::CreateFrameBuffer(VulkanBackBuffer& BackBuffer)
{
    std::vector<VkImageView> Attachments = {BackBuffer.ImageView};

    VkFramebufferCreateInfo FramebufferInfo = {};
    FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    FramebufferInfo.width = BackBuffer.Width;
    FramebufferInfo.height = BackBuffer.Height;
    FramebufferInfo.layers = 1;
    FramebufferInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
    FramebufferInfo.pAttachments = Attachments.data();
    FramebufferInfo.renderPass = m_RenderPass;

    VkFramebuffer FrameBuffer = VK_NULL_HANDLE;
    VULKAN_RESULT(vkCreateFramebuffer(m_Device, &FramebufferInfo, nullptr, &FrameBuffer));
    return FrameBuffer;
}

void VulkanApplication::DestroyFrameBuffer(VkFramebuffer& FrameBuffer)
{
    if (FrameBuffer)
    {
        vkDestroyFramebuffer(m_Device, FrameBuffer, nullptr);
        FrameBuffer = VK_NULL_HANDLE;
    }
}

void VulkanApplication::CreateCommandPool()
{
    VkCommandPoolCreateInfo PoolInfo = {};
    PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    PoolInfo.queueFamilyIndex = m_GraphicsQueue.FamilyIndex;
    PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VULKAN_RESULT(vkCreateCommandPool(m_Device, &PoolInfo, nullptr, &m_CommandPool));

    m_CommandBuffers.resize(NUM_BACKBUFFERS);

    for (uint32_t Index = 0; Index < m_CommandBuffers.size(); ++Index)
    {
        m_CommandBuffers[Index] = CreateCommandBuffer();
    }
}

void VulkanApplication::DestroyCommandPool()
{
    if (m_CommandPool)
    {
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        m_CommandPool = VK_NULL_HANDLE;
    }
}

VkCommandBuffer VulkanApplication::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo AllocateInfo = {};
    AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocateInfo.commandPool = m_CommandPool;
    AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocateInfo.commandBufferCount = 1;

    VkCommandBuffer CommandBuffer;
    VULKAN_RESULT(vkAllocateCommandBuffers(m_Device, &AllocateInfo, &CommandBuffer));
    return CommandBuffer;
}

VkFence VulkanApplication::CreateFence(bool Signaled)
{
    VkFenceCreateInfo FenceInfo = {};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceInfo.flags = Signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VkFence Fence = VK_NULL_HANDLE;
    VULKAN_RESULT(vkCreateFence(m_Device, &FenceInfo, nullptr, &Fence));
    return Fence;
}

void VulkanApplication::DestroyFence(VkFence& Fence)
{
    if (Fence)
    {
        vkDestroyFence(m_Device, Fence, nullptr);
        Fence = VK_NULL_HANDLE;
    }
}

VkSemaphore VulkanApplication::CreateSemaphore()
{
    VkSemaphoreCreateInfo SemaphoreInfo = {};
    SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore Semaphore = VK_NULL_HANDLE;
    VULKAN_RESULT(vkCreateSemaphore(m_Device, &SemaphoreInfo, nullptr, &Semaphore));
    return Semaphore;
}

void VulkanApplication::DestroySemaphore(VkSemaphore& Semaphore)
{
    if (Semaphore)
    {
        vkDestroySemaphore(m_Device, Semaphore, nullptr);
        Semaphore = VK_NULL_HANDLE;
    }
}

void VulkanApplication::CreateSyncObjects()
{
    m_AcquiredImageFences.resize(NUM_BACKBUFFERS);
    for (uint32_t Index = 0; Index < m_AcquiredImageFences.size(); ++Index)
    {
        m_AcquiredImageFences[Index] = CreateFence();
    }

    m_AcquiredImageSemaphores.resize(NUM_BACKBUFFERS);
    for (uint32_t Index = 0; Index < m_AcquiredImageSemaphores.size(); ++Index)
    {
        m_AcquiredImageSemaphores[Index] = CreateSemaphore();
    }

    m_RenderingDoneFences.resize(NUM_BACKBUFFERS);
    for (uint32_t Index = 0; Index < m_RenderingDoneFences.size(); ++Index)
    {
        m_RenderingDoneFences[Index] = CreateFence();
    }

    m_RenderingDoneSemaphores.resize(NUM_BACKBUFFERS);
    for (uint32_t Index = 0; Index < m_RenderingDoneSemaphores.size(); ++Index)
    {
        m_RenderingDoneSemaphores[Index] = CreateSemaphore();
    }
}

void VulkanApplication::DestroySyncObjects()
{
    for (auto& AcquiredFence : m_AcquiredImageFences)
    {
        DestroyFence(AcquiredFence);
    }

    for (auto& AcquiredSemaphore : m_AcquiredImageSemaphores)
    {
        DestroySemaphore(AcquiredSemaphore);
    }

    for (auto& RenderingDoneFence : m_RenderingDoneFences)
    {
        DestroyFence(RenderingDoneFence);
    }

    for (auto& RenderingDoneSemaphore : m_RenderingDoneSemaphores)
    {
        DestroySemaphore(RenderingDoneSemaphore);
    }
}

bool VulkanApplication::AcquireImageIndex(uint32_t* OutImageIndex)
{
    m_ImageSemaphoreIndex = (m_ImageSemaphoreIndex + 1) % m_AcquiredImageSemaphores.size();

    VkResult Result = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT32_MAX,
        m_AcquiredImageSemaphores[m_ImageSemaphoreIndex], nullptr, OutImageIndex);

    if (Result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }

    CHECK(Result == VK_SUCCESS || Result == VK_SUBOPTIMAL_KHR, "Failed to acquire image");

    m_CurrentFrameIndex = *OutImageIndex;

    return true;
}

bool VulkanApplication::Present()
{
    VkPresentInfoKHR PresentInfo = {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.pImageIndices = &m_CurrentFrameIndex;
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &m_SwapChain;
    PresentInfo.waitSemaphoreCount = 1;
    VkSemaphore WaitSemaphores[] = {m_RenderingDoneSemaphores[m_ImageSemaphoreIndex]};
    PresentInfo.pWaitSemaphores = WaitSemaphores;

    VkResult Result = vkQueuePresentKHR(m_PresentQueue.Handle, &PresentInfo);

    if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
    {
        return false;
    }

    CHECK(Result == VK_SUCCESS, "Failed to present image");

    return true;
}

VkCommandBuffer VulkanApplication::BeginCommandBuffer()
{
    m_CommandBufferIndex = (m_CommandBufferIndex + 1) % m_CommandBuffers.size();

    VkCommandBuffer CommandBuffer = m_CommandBuffers[m_CommandBufferIndex];

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

    return CommandBuffer;
}

void VulkanApplication::EndCommandBuffer(VkCommandBuffer& CommandBuffer)
{
    vkEndCommandBuffer(CommandBuffer);
}

void VulkanApplication::Submit(VkCommandBuffer& CommandBuffer)
{
    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;
    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    SubmitInfo.pWaitDstStageMask = &waitStageMask;
    VkSemaphore WaitSemaphores[] = {m_AcquiredImageSemaphores[m_ImageSemaphoreIndex]};
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = WaitSemaphores;
    VkSemaphore SignalSemaphores[] = {m_RenderingDoneSemaphores[m_ImageSemaphoreIndex]};
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = SignalSemaphores;

    VULKAN_RESULT(vkQueueSubmit(
        m_GraphicsQueue.Handle, 1, &SubmitInfo, m_RenderingDoneFences[m_ImageSemaphoreIndex]));

    vkWaitForFences(m_Device, 1, &m_RenderingDoneFences[m_ImageSemaphoreIndex], true, UINT32_MAX);
    vkResetFences(m_Device, 1, &m_RenderingDoneFences[m_ImageSemaphoreIndex]);
}

void VulkanApplication::BeginRenderPass(VkCommandBuffer& CommandBuffer)
{
    VkRenderPassBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    BeginInfo.clearValueCount = 1;
    VkClearValue ClearValue = {0.5f, 0.55f, 0.6f, 1.0f};
    BeginInfo.pClearValues = &ClearValue;
    BeginInfo.framebuffer = m_FrameBuffers[m_CurrentFrameIndex];
    BeginInfo.renderArea.offset.x = 0;
    BeginInfo.renderArea.offset.y = 0;
    BeginInfo.renderArea.extent.width = m_Info.WindowWidth;
    BeginInfo.renderArea.extent.height = m_Info.WindowHeight;
    BeginInfo.renderPass = m_RenderPass;
    vkCmdBeginRenderPass(CommandBuffer, &BeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanApplication::EndRenderPass(VkCommandBuffer& CommandBuffer)
{
    vkCmdEndRenderPass(CommandBuffer);
}