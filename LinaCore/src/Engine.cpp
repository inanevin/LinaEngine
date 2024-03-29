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

#include "Common/Log/Log.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/PlatformTime.hpp"
#include "Common/Tween/TweenManager.hpp"

#include "Core/Application.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/ApplicationDelegate.hpp"

namespace Lina
{
	void Engine::PreInitialize(const SystemInitializationInfo& initInfo)
	{
		TweenManager::s_instance = new TweenManager();

		AddListener(this);

		for (const std::pair<SubsystemType, Subsystem*>& pair : m_subsystems)
			pair.second->PreInitialize(initInfo);
	}

	void Engine::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Engine] -> Initialization.");
		m_initInfo = initInfo;

		for (const std::pair<SubsystemType, Subsystem*>& pair : m_subsystems)
			pair.second->Initialize(initInfo);
	}

	void Engine::CoreResourcesLoaded()
	{
		LINA_TRACE("[Engine] -> Core resources loaded.");
		for (const std::pair<SubsystemType, Subsystem*>& pair : m_subsystems)
			pair.second->CoreResourcesLoaded();
	}

	void Engine::PreShutdown()
	{
		LINA_TRACE("[Engine] -> PreShutdown.");
		m_resourceManager.WaitForAll();
		m_gfxManager.Join();

		for (const std::pair<SubsystemType, Subsystem*>& pair : m_subsystems)
			pair.second->PreShutdown();
	}

	void Engine::Shutdown()
	{
		LINA_TRACE("[Engine] -> Shutdown.");

		delete TweenManager::s_instance;
		TweenManager::s_instance = nullptr;

		// Order matters!
		// Shutdown resource manager first, clean-up subsystems of active resources.
		m_resourceManager.Shutdown();
		m_gfxManager.Shutdown();
		m_audioManager.Shutdown();

		for (const std::pair<SubsystemType, Subsystem*>& pair : m_subsystems)
		{
			if (pair.second == &m_resourceManager || pair.second == &m_gfxManager || pair.second == &m_audioManager)
				continue;

			pair.second->Shutdown();
		}

		RemoveListener(this);
	}

	void Engine::PreTick()
	{
		PROFILER_FUNCTION();

		for (const std::pair<SubsystemType, Subsystem*>& pair : m_subsystems)
			pair.second->PreTick();

		CalculateTime();
	}

	void Engine::Poll()
	{
		PROFILER_FUNCTION();
		m_resourceManager.Poll();
		m_gfxManager.Poll();
	}

	void Engine::Tick()
	{
		PROFILER_FUNCTION();

		const double delta			 = SystemInfo::GetDeltaTime();
		const int64	 fixedTimestep	 = SystemInfo::GetFixedTimestepMicroseonds();
		const double fixedTimestepDb = static_cast<double>(fixedTimestep);
		m_fixedTimestepAccumulator += SystemInfo::GetDeltaTimeMicroSeconds();
		const float deltaF = static_cast<float>(delta);

		// Kick off audio
		auto audioJob = m_executor.Async([&]() { m_audioManager.Tick(deltaF); });

		// Update app.
		TweenManager::Get()->Tick(deltaF);
		m_gfxManager.Tick(deltaF);
		m_audioManager.Tick(deltaF);
		m_app->GetAppDelegate()->OnTick(deltaF);

		// Render
		m_gfxManager.Render();

		audioJob.get();
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
