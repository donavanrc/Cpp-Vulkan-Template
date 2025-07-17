#include "VulkanApplication.h"

#define VULKAN_RESULT(Result)

VulkanApplication::VulkanApplication(const ApplicationInfo& Info)
    : Application(Info), m_Instance(VK_NULL_HANDLE)
{
}

void VulkanApplication::Init()
{
    Application::Init();

    InitGraphics();
};

void VulkanApplication::Destroy()
{
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
    auto InstanceExtensions = GetInstanceExtensions();
    InstanceInfo.enabledExtensionCount = InstanceExtensions.size();
    InstanceInfo.ppEnabledExtensionNames = InstanceExtensions.data();
    auto InstanceLayers = GetValidationLayers();
    InstanceInfo.enabledLayerCount = InstanceLayers.size();
    InstanceInfo.ppEnabledLayerNames = InstanceLayers.data();

    VULKAN_RESULT(vkCreateInstance(&InstanceInfo, nullptr, &m_Instance));

    DEBUG_DISPLAY("API version target: %d.%d.%d", VK_API_VERSION_MAJOR(ApiVersion),
        VK_API_VERSION_MINOR(ApiVersion), VK_API_VERSION_PATCH(ApiVersion));
}
