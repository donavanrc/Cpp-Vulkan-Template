#include "LoadDataApp.h"
#include "Core/Entrypoint.h"

void LoadDataApp::Initialize()
{
    auto Filename = std::filesystem::current_path() / "../Resources/Data/data.txt";
    auto Data = Core::Utility::LoadFile(Filename.string().c_str());
    DEBUG_LOG("%s", Data.c_str());
}

START_APPLICATION(LoadDataApp);