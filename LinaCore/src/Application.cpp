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
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/CommonCore.hpp"
#include "Core/Reflection/CommonReflection.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"

namespace Lina
{
	LinaGX::Instance* Application::s_lgx = nullptr;

	bool Application::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_appDelegate		 = initInfo.appDelegate;
		m_appDelegate->m_app = this;
		LINA_ASSERT(m_appDelegate != nullptr, "Application listener can not be empty!");

		// Setup
		PlatformTime::Initialize();
		SystemInfo::SetAppStartCycles(PlatformTime::GetCPUCycles());
		SystemInfo::SetMainThreadID(SystemInfo::GetCurrentThreadID());
		PROFILER_INIT;
		PROFILER_REGISTER_THREAD("Main");

		// Pre-initialization
		GfxHelpers::InitializeLinaVG();
		s_lgx = GfxHelpers::InitializeLinaGX();

		m_gfxContext.Initialize(&m_resourceManager, s_lgx, &m_guiBackend);
		m_guiBackend.Initialize(&m_resourceManager);
		m_worldProcessor.Initialize(this);

		const bool preInitOK = GetAppDelegate()->PreInitialize();

		if (!preInitOK)
		{
			GfxHelpers::ShutdownLinaVG();
			delete s_lgx;
			s_lgx = nullptr;
			return false;
		}

		// Main window
		auto window = CreateApplicationWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, Vector2ui(initInfo.windowWidth, initInfo.windowHeight), static_cast<uint32>(initInfo.windowStyle));
		window->CenterPositionToCurrentMonitor();
		window->AddListener(this);
		window->SetVisible(true);

		// Initialization
		const bool initOK = GetAppDelegate()->Initialize();

		if (!initOK)
		{
			GfxHelpers::ShutdownLinaVG();
			delete s_lgx;
			s_lgx = nullptr;
			return false;
		}

		return true;
	}

	void Application::Tick()
	{
		PROFILER_FRAME_START();

		PlatformProcess::PumpOSMessages();

		CalculateTime();
		const double delta = SystemInfo::GetDeltaTime();

		s_lgx->TickWindowSystem();
		GetAppDelegate()->PreTick();

		m_resourceManager.SetLocked(true);

		// auto renderJob = m_executor.Async([this]() { m_appDelegate->Render(); });

		m_worldProcessor.Tick(static_cast<float>(delta));
		GetAppDelegate()->Tick(static_cast<float>(delta));

		// renderJob.get();

		m_appDelegate->SyncRender();
		Render();

		m_resourceManager.SetLocked(false);

		if (!SystemInfo::GetAppHasFocus())
			PlatformTime::Sleep(0);
		SystemInfo::SetFrames(SystemInfo::GetFrames() + 1);
		SystemInfo::SetAppTime(SystemInfo::GetAppTime() + SystemInfo::GetDeltaTime());
	}

	void Application::Shutdown()
	{

		s_lgx->Join();
		GetAppDelegate()->PreShutdown();
		m_guiBackend.Shutdown();
		GfxHelpers::ShutdownLinaVG();

		m_gfxContext.Shutdown();
		m_resourceManager.Shutdown();

		GetAppDelegate()->Shutdown();
		DestroyApplicationWindow(LINA_MAIN_SWAPCHAIN);
		delete s_lgx;
		s_lgx = nullptr;

		PROFILER_SHUTDOWN;
		ReflectionSystem::Get().Destroy();
		delete m_appDelegate;
	}

	void Application::OnWindowClose(LinaGX::Window* window)
	{
		if (window->GetSID() == LINA_MAIN_SWAPCHAIN)
			m_exitRequested = true;
	}

	void Application::OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui& sz)
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

	void Application::Render()
	{
		const uint32 frameIndex = s_lgx->GetCurrentFrameIndex();
		s_lgx->StartFrame();

		m_gfxContext.PollUploads(frameIndex);
		m_appDelegate->Render(frameIndex);

		s_lgx->EndFrame();
	}

	void Application::CalculateTime()
	{
		static int64 previous = PlatformTime::GetCPUMicroseconds();
		int64		 current  = PlatformTime::GetCPUMicroseconds();
		int64		 deltaUs  = current - previous;
		previous			  = current;

		if (deltaUs <= 0)
			deltaUs = 16667;
		else if (deltaUs >= 50000)
			deltaUs = 50000;

		const double avgDeltaMicroseconds = SystemInfo::CalculateRunningAverageDT(deltaUs);
		SystemInfo::SetRealDeltaTimeMicroSeconds(deltaUs);
		SystemInfo::SetDeltaTimeMicroSeconds(static_cast<int64>(avgDeltaMicroseconds));

		const float		gameTime	  = SystemInfo::GetAppTimeF();
		static float	lastFPSUpdate = gameTime;
		static uint64	lastFPSFrames = SystemInfo::GetFrames();
		constexpr float measureTime	  = 1.0f;

		if (gameTime > lastFPSUpdate + measureTime)
		{
			const uint64 frames = SystemInfo::GetFrames();
			SystemInfo::SetMeasuredFPS(static_cast<uint32>(static_cast<float>((frames - lastFPSFrames)) / measureTime));
			lastFPSFrames = frames;
			lastFPSUpdate = gameTime;
			LINA_TRACE("FPS: {0} Time: {1}", SystemInfo::GetMeasuredFPS(), static_cast<float>(SystemInfo::GetDeltaTime()) * 1000.0f);
		}
	}

} // namespace Lina
