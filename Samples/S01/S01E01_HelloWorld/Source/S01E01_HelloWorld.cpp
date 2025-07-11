#include "pch.h"

int main()
{
    DEBUG_LOG("%s", "Hello World!");
    DEBUG_DISPLAY("%s", "Display message");
    DEBUG_ERROR("%s", "Error message");
    DEBUG_FATAL("%s", "Fatal error message");
    DEBUG_INFO("%s", "info message");
    DEBUG_WARNING("%s", "Warning message");
    DEBUG_ASSERT(2 + 2 != 4, "%s", "Assertion error message");
    return 0;
}