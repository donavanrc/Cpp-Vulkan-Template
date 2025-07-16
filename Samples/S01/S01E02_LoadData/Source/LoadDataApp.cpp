#include "LoadDataApp.h"
#include "Core/Entrypoint.h"

void LoadDataApp::Init()
{
    auto Filename = std::filesystem::current_path() / "../Resources/Data/data.txt";
    auto Data = Utility::LoadFile(Filename.string().c_str());
    DEBUG_DISPLAY("%s", Data.c_str());
}

START_APPLICATION(LoadDataApp);