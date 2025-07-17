#include "HelloVulkanApp.h"
#include "Core/Entrypoint.h"

HelloVulkanApp::HelloVulkanApp() : VulkanApplication({"Hello Vulkan", 800, 600}) {}

void HelloVulkanApp::OnInit() {}

void HelloVulkanApp::OnDestroy() {}

void HelloVulkanApp::OnUpdate(float DeltaTime) {}

START_APPLICATION(HelloVulkanApp);