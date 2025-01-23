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
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Common/FileSystem/FileSystem.hpp"

namespace Lina
{
	LinaGX::Instance* Application::s_lgx = nullptr;
	Log				  Application::s_log = {};

	bool Application::Initialize(const SystemInitializationInfo& initInfo, String& errString)
	{
		Log::SetLog(&s_log);
		PlatformTime::Initialize();
		SystemInfo::SetAppStartCPUMicroseconds(PlatformTime::GetCPUMicroseconds());
		SystemInfo::SetAppStartCycles(PlatformTime::GetCPUCycles());
		SystemInfo::SetMainThreadID(SystemInfo::GetCurrentThreadID());

		// System time as microseconds
		int32 hours = 0, minutes = 0, seconds = 0;
		FileSystem::GetSystemTimeInts(hours, minutes, seconds);
		const int64_t microseconds = 1000000 * static_cast<int64_t>(hours) * 3600 + static_cast<int64_t>(minutes) * 60 + static_cast<int64_t>(seconds);
		SystemInfo::SetAppStartSystemTimeMicroseconds(microseconds);

		m_appDelegate		 = initInfo.appDelegate;
		m_appDelegate->m_app = this;
		LINA_ASSERT(m_appDelegate != nullptr, "Application listener can not be empty!");

		// Setup

		PROFILER_INIT;
		PROFILER_REGISTER_THREAD("Main");

		// Pre-initialization
		GfxHelpers::InitializeLinaVG();
		s_lgx = GfxHelpers::InitializeLinaGX();

		SystemInfo::SetSwapchainFormat(LinaGX::Format::B8G8R8A8_SRGB);
		SystemInfo::SetHDRFormat(LinaGX::Format::R16G16B16A16_SFLOAT);
		SystemInfo::SetLDRFormat(LinaGX::Format::R8G8B8A8_SRGB);
		SystemInfo::SetDepthFormat(LinaGX::Format::D32_SFLOAT);
		SystemInfo::SetAllowedMSAASamples(4);
		m_renderJoinPossible = true;
		m_gfxContext.Initialize(this);
		m_guiBackend.Initialize(&m_resourceManager);

		const bool preInitOK = GetAppDelegate()->PreInitialize(errString);
		if (!preInitOK)
		{
			m_gfxContext.Shutdown();
			m_guiBackend.Shutdown();
			GfxHelpers::ShutdownLinaVG();
			delete s_lgx;
			s_lgx = nullptr;
			return false;
		}

		// Main window
		auto window = CreateApplicationWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, Vector2ui(initInfo.windowWidth, initInfo.windowHeight), static_cast<uint32>(initInfo.windowStyle));
		window->CenterPositionToCurrentMonitor();
		window->SetVisible(true);

		// Initialization
		const bool initOK = GetAppDelegate()->Initialize(errString);

		if (!initOK)
		{
			m_gfxContext.Shutdown();
			m_guiBackend.Shutdown();
			GfxHelpers::ShutdownLinaVG();
			DestroyApplicationWindow(LINA_MAIN_SWAPCHAIN);
			delete s_lgx;
			s_lgx = nullptr;
			return false;
		}

		m_physicsBackend.Initialize();
		m_audioBackend.Initialize();

		GetAppDelegate()->PostInitialize();
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

		PROFILER_RESET_DRAWINFO();

		/*
			If we recently joined render thread, previous frames produced might be invalid due to resources changes.
			Instead of dropping frames and flickering, we single thread for one frame.
		*/
		const bool singleThreadRenderer = m_joinedRender;
		SystemInfo::SetRendererBehindFrames(singleThreadRenderer ? 0 : 1);
		m_renderJoinPossible = false;
		m_resourceManager.SetLocked(true);
		m_joinedRender = false;

		m_gfxContext.PrepareBindless();

		if (singleThreadRenderer)
		{
			GetAppDelegate()->Tick(static_cast<float>(delta));
			m_appDelegate->SyncRender();

			s_lgx->StartFrame();
			Render();
			s_lgx->EndFrame();
		}
		else
		{
			s_lgx->StartFrame();
			auto renderJob = m_executor.Async([this]() { Render(); });
			GetAppDelegate()->Tick(static_cast<float>(delta));
			renderJob.get();
			s_lgx->EndFrame();

			m_appDelegate->SyncRender();
		}

		m_renderJoinPossible = true;
		m_resourceManager.SetLocked(false);

		if (!SystemInfo::GetAppHasFocus())
			PlatformTime::Sleep(0);
		SystemInfo::SetFrames(SystemInfo::GetFrames() + 1);
		SystemInfo::SetAppTime(SystemInfo::GetAppTime() + SystemInfo::GetDeltaTime());
	}

	void Application::JoinRender()
	{
		if (m_joinedRender)
			return;

		LINA_ASSERT(m_renderJoinPossible, "Can't join render inside render sync locks!");
		s_lgx->Join();
		m_gfxContext.MarkBindlessDirty();
		m_joinedRender = true;
	}

	void Application::Shutdown()
	{
		m_audioBackend.Shutdown();
		m_physicsBackend.Shutdown();

		JoinRender();
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

	LinaGX::Window* Application::CreateApplicationWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow)
	{
		JoinRender();
		auto window = s_lgx->GetWindowManager().CreateApplicationWindow(sid, title, pos.x, pos.y, size.x, size.y, static_cast<LinaGX::WindowStyle>(style), parentWindow);
		return window;
	}

	void Application::DestroyApplicationWindow(StringID sid)
	{
		JoinRender();
		auto* window = s_lgx->GetWindowManager().GetWindow(sid);
		s_lgx->GetWindowManager().DestroyApplicationWindow(sid);
	}

	LinaGX::Window* Application::GetApplicationWindow(StringID sid)
	{
		return s_lgx->GetWindowManager().GetWindow(sid);
	}

	WorldRenderer* Application::CreateAndLoadWorld(ProjectData* project, ResourceID id, LinaGX::Window* window)
	{
		m_resourceManager.LoadResourcesFromProject(project, {id}, {}, id);
		EntityWorld* world = m_resourceManager.GetResource<EntityWorld>(id);
		world->Initialize(&m_resourceManager, window, project);
		world->LoadMissingResources(m_resourceManager, project, {});
		WorldRenderer* wr = new WorldRenderer(&m_gfxContext, &m_resourceManager, world, window->GetSize(), world->GetName());
		m_appDelegate->OnWorldLoaded(wr);
		world->SetPlayMode(PlayMode::Play);
		return wr;
	}

	void Application::UnloadWorld(WorldRenderer* wr)
	{
		EntityWorld* world = wr->GetWorld();
		world->SetPlayMode(PlayMode::None);

		const ResourceID id = world->GetID();
		m_resourceManager.UnloadResourceSpace(world->GetID());
		delete wr;
		m_appDelegate->OnWorldUnloaded(id);
		/*m_resourceManager.UnloadResources({{
			.id	  = world->GetID(),
			.name = world->GetName(),
			.tid  = world->GetTID(),
		}});
		*/
	}

	void Application::Render()
	{
		const uint32 frameIndex = s_lgx->GetCurrentFrameIndex();

		m_gfxContext.UpdateBindless(s_lgx->GetCurrentFrameIndex());
		m_gfxContext.PollUploads(frameIndex);
		m_appDelegate->Render(frameIndex);
	}

	void Application::CalculateTime()
	{
		static int64 previous = PlatformTime::GetCPUMicroseconds();
		int64		 current  = PlatformTime::GetCPUMicroseconds();
		int64		 deltaUs  = current - previous;
		previous			  = current;

		SystemInfo::SetAppTimeMicroseconds(PlatformTime::GetCPUMicroseconds() - SystemInfo::GetAppStartCPUMicroseconds());
		SystemInfo::SetCurrentSystemTimeMicroseconds(SystemInfo::GetAppStartSystemTimeMicroseconds() + SystemInfo::GetAppTimeMicroseconds());

		if (deltaUs <= 0)
			deltaUs = 16667;
		else if (deltaUs >= 50000)
			deltaUs = 50000;

		SystemInfo::SetDeltaTimeMicroseconds(deltaUs);
		SystemInfo::SetSmoothedDeltaMicroseconds(static_cast<int64>(SystemInfo::CalculateRunningAverageDT(deltaUs)));

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
			// LINA_TRACE("FPS: {0} Time: {1}", SystemInfo::GetMeasuredFPS(), static_cast<float>(SystemInfo::GetDeltaTime()) * 1000.0f);
		}
	}

} // namespace Lina
