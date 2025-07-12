#pragma once
#include "pch.h"
#include "Application.h"

namespace Core
{
    bool StartApplication(Application& App);
}

#define START_APPLICATION(AppClass)                                       \
    int main()                                                            \
    {                                                                     \
        AppClass App;                                                     \
        return Core::StartApplication(App) ? EXIT_SUCCESS : EXIT_FAILURE; \
    }