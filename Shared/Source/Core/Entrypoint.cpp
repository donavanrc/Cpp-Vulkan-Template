#include "Entrypoint.h"

bool StartApplication(IApplication& App)
{
    try
    {
        App.Initialize();
        App.Destroy();
    }
    catch (const std::exception& Exception)
    {
        PRINT_ERROR(Exception.what());
        return false;
    }

    return true;
}
