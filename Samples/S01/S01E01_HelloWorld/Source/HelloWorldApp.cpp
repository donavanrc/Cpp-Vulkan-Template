#include "HelloWorldApp.h"

void HelloWorldApp::Initialize()
{
    DEBUG_INFO("App initialized");
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
