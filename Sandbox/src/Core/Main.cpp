#include "Lina.hpp"
#include "Lina/ReflectionRegistry.hpp"
#include "Game/GameManager.hpp"

int main(int argc, char** argv)
{
    Lina::Application* application = new Lina::Application();

    Lina::WindowProperties windowProps = Lina::WindowProperties{
        .title      = "Lina Engine - Demo Sandbox Project",
        .width      = 1920,
        .height     = 1080,
        .vsync      = Lina::VsyncMode::None,
        .decorated  = true,
        .resizable  = true,
        .fullscreen = false,
    };

    Lina::InitInfo appInfo = Lina::InitInfo{
        .appName          = "Lina Sandbox Game",
        .packagePass      = L"1234",
        .appMode          = Lina::ApplicationMode::Editor,
        .windowProperties = windowProps,
        .preferredGPU     = Lina::PreferredGPUType::Discrete,
    };

    GameManager game;
    Lina::RegisterReflectedTypes();
    application->Initialize(appInfo);
    game.Initialize();
    application->Run();
    delete application;

    Lina::Application::Cleanup();
}
