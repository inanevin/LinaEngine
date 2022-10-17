#include "Lina.hpp"
#include "Lina/ReflectionRegistry.hpp"
#include "Game/GameManager.hpp"

int main(int argc, char** argv)
{
    Lina::Application* application = new Lina::Application();

    Lina::WindowProperties windowProps = Lina::WindowProperties{
        .title      = "Lina Engine - Demo Sandbox Project",
        .width      = 2560,
        .height     = 1440,
        .vsync      = Lina::VsyncMode::None,
        .decorated  = true,
        .resizable  = true,
        .fullscreen = false,
    };

    Lina::InitInfo appInfo = Lina::InitInfo{
        .appName          = "Lina Sandbox Game",
        .appMode          = Lina::ApplicationMode::Editor,
        .windowProperties = windowProps,
        .preferredGPU     = Lina::PreferredGPUType::Integrated,
    };

    GameManager game;
    Lina::RegisterReflectedTypes();
    application->Initialize(appInfo);
    game.Initialize();
    application->Run();
    delete application;

    Lina::Application::Cleanup();
}
