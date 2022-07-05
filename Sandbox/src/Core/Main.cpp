#include "Lina.hpp"

int main(int argc, char** argv)
{
    Lina::Application* application = new Lina::Application();

    Lina::WindowProperties windowProps = Lina::WindowProperties{
        .title      = "Lina Engine - Demo Sandbox Project",
        .width      = 600,
        .height     = 600,
        .vsync      = Lina::VsyncMode::None,
        .decorated  = true,
        .resizable  = true,
        .fullscreen = false,
    };

    Lina::ApplicationInfo appInfo = Lina::ApplicationInfo{
        .appName          = "Lina Sandbox Game",
        .packagePass      = L"1234",
        .build            = LINA_BUILD,
        .versionMajor     = LINA_MAJOR,
        .versionMinor     = LINA_MINOR,
        .versionPatch     = LINA_PATCH,
        .appMode          = Lina::ApplicationMode::Editor,
        .windowProperties = windowProps,
    };

    application->Initialize(appInfo);
    application->Run();
    delete application;

    Lina::Application::Cleanup();
}
