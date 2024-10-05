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

#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Common/Platform/PlatformTime.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/CommonCore.hpp"
#include "Core/Reflection/CommonReflection.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"

namespace Lina
{
    LinaGX::Instance* Application::s_lgx = nullptr;

	void Application::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_appDelegate		 = initInfo.appDelegate;
		m_appDelegate->m_app = this;
		LINA_ASSERT(m_appDelegate != nullptr, "Application listener can not be empty!");

		// Setup
		PlatformTime::Initialize();
		SystemInfo::SetAppStartCycles(PlatformTime::GetCPUCycles());
		SystemInfo::SetMainThreadID(SystemInfo::GetCurrentThreadID());
		SetFixedTimestep(10000);
		SetFixedTimestep(true);
		PROFILER_INIT;
		PROFILER_REGISTER_THREAD("Main");

		// Pre-initialization
        GfxHelpers::InitializeLinaVG();
        s_lgx = GfxHelpers::InitializeLinaGX();
        GetAppDelegate()->PreInitialize();

		// Main window
		auto window = CreateApplicationWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, Vector2ui(initInfo.windowWidth, initInfo.windowHeight), static_cast<uint32>(initInfo.windowStyle));
		window->CenterPositionToCurrentMonitor();
		window->AddListener(this);
		window->SetVisible(true);

		// Initialization
        GetAppDelegate()->Initialize();
	}

	void Application::LoadPlugins()
	{
		// PlatformProcess::LoadPlugin("GamePlugin.dll", m_engine.GetInterface(), &m_engine);
	}

	void Application::UnloadPlugins()
	{
		// PlatformProcess::UnloadPlugin("GamePlugin.dll", &m_engine);
	}

	void Application::Tick()
	{
        PROFILER_FRAME_START();
        PROFILER_FUNCTION();

        // Time.
        CalculateTime();

        // PreTick
        s_lgx->TickWindowSystem();
        GetAppDelegate()->PreTick();
       
        // Tick
        const double delta             = SystemInfo::GetDeltaTime();
        const float     deltaF             = SystemInfo::GetDeltaTimeF();
        const int64     fixedTimestep     = SystemInfo::GetFixedTimestepMicroseonds();
        const double fixedTimestepDb = static_cast<double>(fixedTimestep);
        m_fixedTimestepAccumulator += SystemInfo::GetDeltaTimeMicroSeconds();
        GetAppDelegate()->Tick(deltaF);
		SystemInfo::SetFrames(SystemInfo::GetFrames() + 1);
		SystemInfo::SetAppTime(SystemInfo::GetAppTime() + SystemInfo::GetDeltaTime());

		// Yield-CPU check.
		if (!SystemInfo::GetAppHasFocus())
			PlatformTime::Sleep(0);
	}

	void Application::Render()
	{
		PROFILER_FUNCTION();
		m_appDelegate->Render();
	}

	void Application::Shutdown()
	{
        GetLGX()->Join();
        GetAppDelegate()->PreShutdown();
        GfxHelpers::ShutdownLinaVG();
        
        GetAppDelegate()->Shutdown();
        DestroyApplicationWindow(LINA_MAIN_SWAPCHAIN);
        delete s_lgx;
        s_lgx = nullptr;

		PROFILER_SHUTDOWN;
		ReflectionSystem::Get().Destroy();
		delete m_appDelegate;
	}

	void Application::SetFrameCap(int64 microseconds)
	{
		SystemInfo::SetFrameCap(microseconds);
	}
	void Application::SetFixedTimestep(int64 microseconds)
	{
		SystemInfo::SetFixedTimestep(microseconds);
	}

	void Application::OnWindowClose(LinaGX::Window* window)
	{
        if(window->GetSID() == LINA_MAIN_SWAPCHAIN)
            m_exitRequested = true;
	}

    void Application::OnWindowSizeChanged(LinaGX::Window *window, const LinaGX::LGXVector2ui& sz)
    {
        GetAppDelegate()->OnWindowSizeChanged(window, sz);
    }

    LinaGX::Window* Application::CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow)
    {
        s_lgx->Join();
        auto window = s_lgx->GetWindowManager().CreateApplicationWindow(sid, title, pos.x, pos.y, size.x, size.y, static_cast<LinaGX::WindowStyle>(style), parentWindow);
        window->AddListener(this);
        return window;
    }
    
    void Application::DestroyApplicationWindow(StringID sid)
    {
        s_lgx->Join();
        auto* window = s_lgx->GetWindowManager().GetWindow(sid);
        window->RemoveListener(this);
        s_lgx->GetWindowManager().DestroyApplicationWindow(sid);
    }
    
    LinaGX::Window* Application::GetApplicationWindow(StringID sid)
    {
        return s_lgx->GetWindowManager().GetWindow(sid);
    }

    void Application::CalculateTime()
    {
        static int64 previous = PlatformTime::GetCPUMicroseconds();
        int64         current  = PlatformTime::GetCPUMicroseconds();
        int64         deltaUs  = current - previous;
        
        const int64 frameCap = SystemInfo::GetFrameCapMicroseconds();
        
        if (frameCap > 0 && deltaUs < frameCap)
        {
            const int64 throttleAmount = frameCap - deltaUs;
            m_frameCapAccumulator += throttleAmount;
            const int64 throttleBegin = PlatformTime::GetCPUMicroseconds();
            PlatformTime::Throttle(m_frameCapAccumulator);
            const int64 totalThrottle = PlatformTime::GetCPUMicroseconds() - throttleBegin;
            m_frameCapAccumulator -= totalThrottle;
            SystemInfo::SetThrottleTime(totalThrottle);
            
            current = PlatformTime::GetCPUMicroseconds();
            deltaUs = current - previous;
        }
        
        previous = current;
        
        if (deltaUs <= 0)
            deltaUs = 16667;
        else if (deltaUs >= 50000)
            deltaUs = 50000;
        
        const double avgDeltaMicroseconds = SystemInfo::CalculateRunningAverageDT(deltaUs);
        SystemInfo::SetRealDeltaTimeMicroSeconds(deltaUs);
        SystemInfo::SetDeltaTimeMicroSeconds(static_cast<int64>(avgDeltaMicroseconds));
        
        const float        gameTime      = SystemInfo::GetAppTimeF();
        static float    lastFPSUpdate = gameTime;
        static uint64    lastFPSFrames = SystemInfo::GetFrames();
        constexpr float measureTime      = 1.0f;
        
        if (gameTime > lastFPSUpdate + measureTime)
        {
            const uint64 frames = SystemInfo::GetFrames();
            SystemInfo::SetMeasuredFPS(static_cast<uint32>(static_cast<float>((frames - lastFPSFrames)) / measureTime));
            lastFPSFrames = frames;
            lastFPSUpdate = gameTime;
            LINA_TRACE("FPS: {0} Time: {1} Idle: {2}", SystemInfo::GetMeasuredFPS(), SystemInfo::GetDeltaTimeF() * 1000.0f, (SystemInfo::GetThrottleTime()) * 0.001f);
        }
    }

} // namespace Lina
