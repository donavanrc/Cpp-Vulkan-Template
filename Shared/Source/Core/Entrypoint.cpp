#include "Entrypoint.h"

namespace Core
{
    bool StartApplication(Application& App)
    {
        try
        {
            App.Initialize();
            App.Run();
            App.Destroy();
        }
        catch (const std::exception& Exception)
        {
            DEBUG_ERROR(Exception.what());
            return false;
        }

        return true;
    }
}  // namespace Core