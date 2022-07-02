#include "Lina.hpp"

int main(int argc, char** argv)
{
    Lina::Application*    application = new Lina::Application();
    Lina::ApplicationInfo appInfo;
    appInfo.m_appMode                        = Lina::ApplicationMode::Editor;
    appInfo.m_appName                        = "Lina Sandbox Game";
    appInfo.m_bundleName                     = "Example";
    appInfo.m_packagePass                    = L"1234";
    appInfo.m_build                          = 0;
    appInfo.m_appMajor                       = 1;
    appInfo.m_appMinor                       = 0;
    appInfo.m_appPatch                       = 0;
    appInfo.m_windowProperties.m_width       = 1920;
    appInfo.m_windowProperties.m_height      = 1080;
    appInfo.m_windowProperties.m_windowState = Lina::WindowState::Maximized;
    appInfo.m_windowProperties.m_decorated   = false;
    appInfo.m_windowProperties.m_resizable   = true;
    appInfo.m_windowProperties.m_fullscreen  = false;
    appInfo.m_windowProperties.m_vsync       = 0;
    appInfo.m_windowProperties.m_msaaSamples = 0;
    appInfo.m_windowProperties.m_title       = "Lina Engine - Demo Sandbox Project";
    application->Initialize(appInfo);
    application->Run();

    delete application;
}
