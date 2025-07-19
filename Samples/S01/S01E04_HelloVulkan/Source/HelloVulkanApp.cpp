#include "HelloVulkanApp.h"
#include "Core/Entrypoint.h"

HelloVulkanApp::HelloVulkanApp() : VulkanApplication({"Hello Vulkan", 800, 600}) {}

void HelloVulkanApp::OnInit() {}

void HelloVulkanApp::OnDestroy() {}

void HelloVulkanApp::OnUpdate(float DeltaTime)
{
    uint32_t ImageIndex = 0;
    if (!AcquireImageIndex(&ImageIndex))
    {
        return;
    }

    VkCommandBuffer CommandBuffer = BeginCommandBuffer();

    BeginRenderPass(CommandBuffer);

    EndRenderPass(CommandBuffer);

    EndCommandBuffer(CommandBuffer);

    Submit(CommandBuffer);

    Present();
}

START_APPLICATION(HelloVulkanApp);