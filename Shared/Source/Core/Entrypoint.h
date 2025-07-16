#pragma once
#include "pch.h"
#include "IApplication.h"

bool StartApplication(IApplication& App);

#define START_APPLICATION(AppClass)                                 \
    int main()                                                      \
    {                                                               \
        AppClass App;                                               \
        return StartApplication(App) ? EXIT_SUCCESS : EXIT_FAILURE; \
    }