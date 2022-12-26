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

#include "EntryPoint.hpp"
#include "Log/Log.hpp"
#include "Core/Application.hpp"
#include "Game/GameManager.hpp"

#include "EventSystem/EventSystem.hpp"
#include "EventSystem/MainLoopEvents.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include "Graphics/Platform/Win32/Win32Window.hpp"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    // Win32 stuff
    MSG msg    = {0};
    msg.wParam = 0;

#ifndef LINA_PRODUCTION
    if (AllocConsole() == FALSE)
        Lina::Log::LogMessage(Lina::LogLevel::Error, "[Entry Point] -> Couldn't allocate console!");
#endif

    // Prepare & init.
    Lina::Application* app = new Lina::Application();
    Lina::InitInfo     initInfo;
    Lina::GameManager* gm = nullptr;
    Lina::Sandbox_PrepareLinaInit(initInfo, gm);

    if (gm == nullptr)
    {
        Lina::Log::LogMessage(Lina::LogLevel::Error, "[Entry Point] -> Sandbox Application needs to pass in a valid GameManager instance!");
        return 0;
    }

    // Initialize main Lina Systems.
    app->Initialize(initInfo, gm);

    // Retrieve window.
    auto* window  = Lina::Graphics::Win32Window::GetWin32();
    HWND  whandle = window->GetWindowPtr();

    app->Start();

    while (app->IsRunning())
    {
        app->PrePoll();

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
            break;

        app->Tick();
    }

    app->Shutdown();
    Lina::Application::Cleanup(app);
    delete app;

    FreeConsole();

    return 0;
}

#else

int main(int argc, char** argv)
{
    // Prepare & init.
    Lina::Application* app = new Lina::Application();
    Lina::InitInfo     initInfo;
    Lina::GameManager* gm = nullptr;
    Lina::Sandbox_PrepareLinaInit(initInfo, gm);

    if (gm == nullptr)
    {
        Lina::Log::LogMessage(Lina::LogLevel::Error, "[Entry Point] -> Sandbox Application needs to pass in a valid GameManager instance!");
        return 0;
    }

    // Initialize main Lina Systems.
    app->Initialize(initInfo, gm);

    while (app->IsRunning())
    {
        // Window poll
        LINA_ASSERT(false, "Not implemented yet!");

        // Tick
        app->Tick();
    }

    app->Shutdown();
    Lina::Application::Cleanup(app);
    delete app;
}
#endif