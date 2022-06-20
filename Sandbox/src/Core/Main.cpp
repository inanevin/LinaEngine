/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Lina.hpp"

int main(int argc, char** argv)
{
    Lina::Application* application = new Lina::Application();
    Lina::ApplicationInfo  appInfo;
    appInfo.m_appMode                        = Lina::ApplicationMode::Standalone;
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
