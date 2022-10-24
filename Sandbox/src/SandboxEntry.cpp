#include "Lina.hpp"
#include "Lina/ReflectionRegistry.hpp"
#include "Game/SandboxGameManager.hpp"

namespace Lina
{
    void Sandbox_PrepareLinaInit(InitInfo& initInfo, GameManager*& gm)
    {
        // Initialize application info
        Lina::WindowProperties windowProps = Lina::WindowProperties{
            .title      = "Lina Engine - Demo Sandbox Project",
            .width      = 1920,
            .height     = 1080,
            .vsync      = Lina::VsyncMode::None, // fifo
            .decorated  = true,
            .resizable  = true,
            .fullscreen = false,
        };

        initInfo = Lina::InitInfo{
            .appName          = "Lina Sandbox Game",
            .appMode          = Lina::ApplicationMode::Editor,
            .windowProperties = windowProps,
            .preferredGPU     = Lina::PreferredGPUType::Integrated,
        };

        // Reflection registry
        Lina::RegisterReflectedTypes();

        // Finally, pass in our own game manager.
        gm = new Sandbox::SandboxGameManager();
    }

} // namespace Lina
