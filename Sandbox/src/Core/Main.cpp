#include "Lina.hpp"

int main(int argc, char** argv)
{
    Lina::Application* application = new Lina::Application();

    Lina::WindowProperties windowProps = Lina::WindowProperties{
        .title      = "Lina Engine - Demo Sandbox Project",
        .width      = 1200,
        .height     = 1200,
        .vsync      = Lina::VsyncMode::Adaptive,
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

    application->Initialize(appInfo);
    application->Run();
    delete application;

    Lina::Application::Cleanup();
}
