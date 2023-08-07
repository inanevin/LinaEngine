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
#include "Log/Log.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/SystemInfo.hpp"
#include "Core/PlatformTime.hpp"
#include "Core/PlatformProcess.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/Font.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Math/Math.hpp"

#define DEFAULT_RATE 1.0f / 60.0f

namespace Lina
{
	void Engine::PreInitialize(const SystemInitializationInfo& initInfo)
	{
		m_gfxManager = new GfxManager(initInfo, this);
		m_resourceManager.AddListener(this);
		m_lgxWrapper.PreInitialize(initInfo);
	}

	void Engine::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Application] -> Initialization.");
		m_initInfo = initInfo;

		for (auto [type, sys] : m_subsystems)
			sys->Initialize(initInfo);
	}

	void Engine::Shutdown()
	{
		LINA_TRACE("[Application] -> Shutdown.");

		m_resourceManager.WaitForAll();

		if (m_gfxManager)
			m_gfxManager->Join();

		for (auto [type, sys] : m_subsystems)
			sys->PreShutdown();

		m_resourceManager.RemoveListener(this);
		m_levelManager.Shutdown();
		m_resourceManager.Shutdown();
		m_gfxManager->Shutdown();
		m_audioManager.Shutdown();
		m_lgxWrapper.Shutdown();

		delete m_gfxManager;
	}

	void Engine::PreTick()
	{
		PROFILER_FUNCTION();

		if (m_gfxManager)
			m_gfxManager->WaitForSwapchains();

		CalculateTime();
	}

	void Engine::Poll()
	{
		PROFILER_FUNCTION();
		m_resourceManager.Tick();
		m_lgxWrapper.GetLGX()->PollWindow();
	}

	void Engine::Tick()
	{
		PROFILER_FUNCTION();

		const double delta			 = SystemInfo::GetDeltaTime();
		const int64	 fixedTimestep	 = SystemInfo::GetFixedTimestepMicroseonds();
		const double fixedTimestepDb = static_cast<double>(fixedTimestep);
		m_fixedTimestepAccumulator += SystemInfo::GetDeltaTimeMicroSeconds();

		auto renderJob = m_executor.Async([&]() { m_gfxManager->Render(); });

		while (m_fixedTimestepAccumulator >= fixedTimestep)
		{
			m_levelManager.Simulate(static_cast<float>(fixedTimestepDb * 0.000001 * SystemInfo::GetTimescale()));
			m_fixedTimestepAccumulator -= fixedTimestep;
		}

		m_levelManager.Tick(static_cast<float>(delta * SystemInfo::GetTimescale()));

		const double interpolationAlpha = static_cast<double>(m_fixedTimestepAccumulator) / fixedTimestepDb;

		if (m_gfxManager)
			m_gfxManager->Tick(static_cast<float>(SystemInfo::GetInterpolationAlpha()));

		// auto audioJob  = m_executor.Async([&]() { m_audioManager.Tick(delta); });
		//	audioJob.get();
		//	m_levelManager.WaitForSimulation();

		renderJob.get();

		if (m_gfxManager)
			m_gfxManager->Sync();

		// if (m_input.GetKeyDown(LINA_KEY_1))
		// {
		// 	m_windowManager.SetVsync(VsyncMode::None);
		// }
		//
		// if (m_input.GetKeyDown(LINA_KEY_2))
		// {
		// 	m_windowManager.SetVsync(VsyncMode::EveryVBlank);
		// }
		//
		// if (m_input.GetKeyDown(LINA_KEY_3))
		// {
		// 	m_windowManager.SetVsync(VsyncMode::EverySecondVBlank);
		// }
		//
		// if (m_input.GetKeyDown(LINA_KEY_4))
		// {
		// 	m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::Fullscreen);
		// }
		//
		// if (m_input.GetKeyDown(LINA_KEY_K))
		// {
		// 	OnCriticalGfxError();
		// }
		//
		// if (m_input.GetKeyDown(LINA_KEY_5))
		// {
		// 	m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::Windowed);
		// }
		// if (m_input.GetKeyDown(LINA_KEY_6))
		// {
		// 	m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::WindowedNoResize);
		// }
		// if (m_input.GetKeyDown(LINA_KEY_7))
		// {
		// 	m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::Borderless);
		// }
		// if (m_input.GetKeyDown(LINA_KEY_8))
		// {
		// 	m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::BorderlessNoResize);
		// }
		// if (m_input.GetKeyDown(LINA_KEY_9))
		// {
		// 	m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::None);
		// }
	}

	void Engine::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_ResourceLoaded)
		{
			String* path = static_cast<String*>(ev.pParams[0]);
			LINA_TRACE("[Resource] -> Loaded resource: {0}", path->c_str());
		}
		else if (eventType & EVS_ResourceUnloaded)
		{
			String* path = static_cast<String*>(ev.pParams[0]);
			LINA_TRACE("[Resource] -> Unloaded resource: {0}", path->c_str());
		}
	}

	void Engine::OnCriticalGfxError()
	{
		auto allTexturesRaw = m_resourceManager.GetAllResourcesRaw<Texture>(false);
		auto allModelsRaw	= m_resourceManager.GetAllResourcesRaw<Model>(false);
		auto allFontsRaw	= m_resourceManager.GetAllResourcesRaw<Font>(false);
		auto allShadersRaw	= m_resourceManager.GetAllResourcesRaw<Shader>(false);

		Vector<ResourceIdentifier> reloadResources;
		Vector<ResourceIdentifier> unloadResources;
		reloadResources.reserve(allTexturesRaw.size() + allFontsRaw.size() + allModelsRaw.size() + allShadersRaw.size());
		unloadResources.reserve(allTexturesRaw.size() + allFontsRaw.size() + allModelsRaw.size() + allShadersRaw.size());

		for (auto res : allTexturesRaw)
		{
			if (!m_resourceManager.IsCoreResource(res->GetSID()) && !m_resourceManager.IsPriorityResource(res->GetSID()))
				reloadResources.push_back(ResourceIdentifier(res->GetPath(), res->GetTID(), res->GetSID()));

			unloadResources.push_back(ResourceIdentifier(res->GetPath(), res->GetTID(), res->GetSID()));
		}
		
		for (auto res : allFontsRaw)
		{
			if (!m_resourceManager.IsCoreResource(res->GetSID()) && !m_resourceManager.IsPriorityResource(res->GetSID()))
				reloadResources.push_back(ResourceIdentifier(res->GetPath(), res->GetTID(), res->GetSID()));

			unloadResources.push_back(ResourceIdentifier(res->GetPath(), res->GetTID(), res->GetSID()));
		}
		for (auto res : allShadersRaw)
		{
			if (!m_resourceManager.IsCoreResource(res->GetSID()) && !m_resourceManager.IsPriorityResource(res->GetSID()))
				reloadResources.push_back(ResourceIdentifier(res->GetPath(), res->GetTID(), res->GetSID()));

			unloadResources.push_back(ResourceIdentifier(res->GetPath(), res->GetTID(), res->GetSID()));
		}

		m_gfxManager->Join();
		m_gfxManager->PreShutdown();
		m_resourceManager.UnloadResources(unloadResources);
		m_gfxManager->Shutdown();
		delete m_gfxManager;
		m_gfxManager = new GfxManager(m_initInfo, this);
		m_resourceManager.LoadResources(m_resourceManager.GetPriorityResources());
		m_resourceManager.WaitForAll();
		m_resourceManager.LoadResources(m_resourceManager.GetCoreResources());
		m_resourceManager.WaitForAll();
		if (!reloadResources.empty())
		{
			m_resourceManager.LoadResources(reloadResources);
			m_resourceManager.WaitForAll();
		}

		m_gfxManager->Initialize(m_initInfo);
	}

	void Engine::CalculateTime()
	{
		static int64 previous = PlatformTime::GetCPUMicroseconds();
		int64		 current  = PlatformTime::GetCPUMicroseconds();
		int64		 deltaUs  = current - previous;

		const int64 frameCap = SystemInfo::GetFrameCapMicroseconds();

		if (frameCap > 0 && deltaUs < frameCap)
		{
			const int64 throttleAmount = frameCap - deltaUs;
			m_frameCapAccumulator += throttleAmount;
			const int64 throttleBegin = PlatformTime::GetCPUMicroseconds();
			PlatformTime::Throttle(m_frameCapAccumulator);
			const int64 totalThrottle = PlatformTime::GetCPUMicroseconds() - throttleBegin;
			m_frameCapAccumulator -= totalThrottle;
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

			LINA_TRACE("[FPS] : {0}", SystemInfo::GetMeasuredFPS());
			LINA_TRACE("[DT]: {0}", SystemInfo::GetDeltaTime());
		}
	}

} // namespace Lina
