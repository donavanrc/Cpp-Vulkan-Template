#include "HelloWorldApp.h"

void HelloWorldApp::Initialize()
{
    DEBUG_INFO("App initialized");

    DEBUG_DISPLAY("%s", "Display message.");
    DEBUG_ERROR("%s", "Error message.");
    DEBUG_INFO("%s", "Info message.");
    DEBUG_LOG("%s", "Log message.");
    DEBUG_WARNING("%s", "Warning message.");
    DEBUG_ASSERT(false, "Assertion error.");
};
void HelloWorldApp::Run()
{
    DEBUG_INFO("App running...");
};
void HelloWorldApp::Destroy()
{
    DEBUG_INFO("App destroyed");
};

START_APPLICATION(HelloWorldApp);
