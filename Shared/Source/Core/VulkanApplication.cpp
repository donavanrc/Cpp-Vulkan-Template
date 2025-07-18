#include "VulkanApplication.h"

#define VULKAN_RESULT(Result) CHECK(Result == VK_SUCCESS, "Vulkan Error (%d): %s", Result, #Result)

VulkanApplication::VulkanApplication(const ApplicationInfo& Info)
    : Application(Info),
      m_Instance(VK_NULL_HANDLE),
      m_DebugMessenger(VK_NULL_HANDLE),
      m_Device(VK_NULL_HANDLE)
{
}

void VulkanApplication::Init()
{
    Application::Init();

    InitGraphics();
};

void VulkanApplication::Destroy()
{
    if (m_Device)
    {
        vkDestroyDevice(m_Device, nullptr);
        m_Device = VK_NULL_HANDLE;
    }

    DestroyDebugMessenger();

    if (m_Instance)
    {
        vkDestroyInstance(m_Instance, nullptr);
        m_Instance = VK_NULL_HANDLE;
    }

    Application::Destroy();
};

void VulkanApplication::InitGraphics()
{
    CreateInstance();
    CreateDebugMessenger();
    SelectPhysicalDevice();
    CreateDevice();
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

VulkanQueue VulkanApplication::CreateQueue(uint32_t FamilyIndex)
{
    VulkanQueue Queue{.FamilyIndex = FamilyIndex};
    vkGetDeviceQueue(m_Device, Queue.FamilyIndex, 0, &Queue.Handle);
    return Queue;
}