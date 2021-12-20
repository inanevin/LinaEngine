/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Core/Engine.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/PhysicsComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "Audio/Audio.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/Timer.hpp"
#include "Core/PhysicsCommon.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina
{
	Engine* Engine::s_engine = nullptr;

	

	double Engine::GetElapsedTime()
	{
		return Utility::GetCPUTime() - m_startTime;
	}

	void Engine::SetPlayMode(bool enabled)
	{
		m_isInPlayMode = enabled;
		m_eventSystem.Trigger<Event::EPlayModeChanged>(Event::EPlayModeChanged{ enabled });

		if (!m_isInPlayMode && m_paused)
			SetIsPaused(false);
	}

	void Engine::SetIsPaused(bool paused)
	{
		if (paused && !m_isInPlayMode) return;
		m_paused = paused;
		m_eventSystem.Trigger<Event::EPauseModeChanged>(Event::EPauseModeChanged{ m_paused });
	}

	void Engine::SkipNextFrame()
	{
		if (!m_isInPlayMode) return;

		m_shouldSkipFrame = true;
	}



	void Engine::Initialize(ApplicationInfo& appInfo)
	{
		Event::EventSystem::s_eventSystem = &m_eventSystem;
		ECS::Registry::s_ecs = &m_ecs;
		Graphics::WindowBackend::s_openglWindow = &m_window;
		Graphics::RenderEngineBackend::s_renderEngine = &m_renderEngine;
		Physics::PhysicsEngineBackend::s_physicsEngine = &m_physicsEngine;
		Input::InputEngineBackend::s_inputEngine = &m_inputEngine;
		Resources::ResourceManager::s_resourceManager = &m_resourceManager;
		Audio::AudioEngineBackend::s_audioEngine = &m_audioEngine;

		m_appInfo = appInfo;
		m_eventSystem.Initialize();
		m_inputEngine.Initialize();

		// Build main window.
		bool windowCreationSuccess = m_window.CreateContext(appInfo);
		if (!windowCreationSuccess)
		{
			LINA_ERR("Window Creation Failed!");
			return;
		}

		// Set event callback for main window.
		m_renderEngine.SetScreenDisplay(Vector2::Zero, m_window.GetSize());

		m_renderEngine.ConnectEvents();
		m_audioEngine.Initialize();
		m_ecs.Initialize();
		m_physicsEngine.Initialize(m_appInfo.m_appMode);
		m_renderEngine.Initialize(m_appInfo.m_appMode);
		
		// Register ECS components for cloning & serialization functionality.
		m_ecs.RegisterComponent<ECS::EntityDataComponent>();
		m_ecs.RegisterComponent<ECS::FreeLookComponent>();
		m_ecs.RegisterComponent<ECS::PhysicsComponent>();
		m_ecs.RegisterComponent<ECS::CameraComponent>();
		m_ecs.RegisterComponent<ECS::PointLightComponent>();
		m_ecs.RegisterComponent<ECS::SpotLightComponent>();
		m_ecs.RegisterComponent<ECS::DirectionalLightComponent>();
		m_ecs.RegisterComponent<ECS::MeshRendererComponent>();
		m_ecs.RegisterComponent<ECS::ModelRendererComponent>();
		m_ecs.RegisterComponent<ECS::SpriteRendererComponent>();
	}

	void Engine::StartLoadingResources()
	{
		if (m_appInfo.m_appMode == ApplicationMode::Editor)
			m_resourceManager.LoadEditorResources();
		else
			m_resourceManager.ImportResourceBundle("", m_appInfo.m_bundleName);

		m_renderEngine.ConstructEngineMaterials();

		// Initialize any listeners.
		m_eventSystem.Trigger<Event::EInitialize>(Event::EInitialize{});

		m_deltaTimeArray.fill(-1.0);
		m_isInPlayMode = true;

		if (m_appInfo.m_appMode == ApplicationMode::Editor)
			m_audioEngine.PlayOneShot(Audio::Audio::GetAudio("resources/editor/audio/lina_init.wav"));
	}

	void Engine::Run()
	{
		m_running = true;
		m_startTime = Utility::GetCPUTime();
		m_physicsAccumulator = 0.0f;

		PROFILER_MAIN_THREAD;
		PROFILER_ENABLE;

		int frames = 0;
		int updates = 0;
		double totalFPSTime = 0.0f;
		double previousFrameTime;
		double currentFrameTime = 0.0f;

		// Starting game.
		m_eventSystem.Trigger<Event::EStartGame>(Event::EStartGame{});

		while (m_running)
		{
			previousFrameTime = currentFrameTime;
			currentFrameTime = GetElapsedTime();
			m_rawDeltaTime = (currentFrameTime - previousFrameTime);
			m_smoothDeltaTime = SmoothDeltaTime(m_rawDeltaTime);

			m_inputEngine.Tick();
			updates++;
			LINA_TIMER_START("Update MS");
			UpdateGame((float)m_rawDeltaTime);
			LINA_TIMER_STOP("Update MS");
			m_updateTime = Lina::Timer::GetTimer("UPDATE MS").GetDuration();

			LINA_TIMER_START("RENDER MS");
			DisplayGame(1.0f);
			LINA_TIMER_STOP("RENDER MS");
			m_renderTime = Lina::Timer::GetTimer("RENDER MS").GetDuration();
			frames++;

			double now = GetElapsedTime();
			// Calculate FPS, UPS.
			if (now - totalFPSTime >= 1.0)
			{
				m_frameTime = m_rawDeltaTime * 1000;
				m_currentFPS = frames;
				m_currentUPS = updates;
				totalFPSTime += 1.0;
				frames = 0;
				updates = 0;
			}

			if (m_firstRun)
				m_firstRun = false;
		}

		// Ending game.
		m_eventSystem.Trigger<Event::EEndGame>(Event::EEndGame{});

		// Shutting down.
		m_ecs.Shutdown();
		m_physicsEngine.Shutdown();
		m_renderEngine.Shutdown();
		m_audioEngine.Shutdown();
		m_inputEngine.Shutdown();
		m_resourceManager.Shutdown();
		m_eventSystem.Trigger<Event::EShutdown>(Event::EShutdown{});
		m_eventSystem.Shutdown();

		PROFILER_DUMP("profile.prof");
		Timer::UnloadTimers();

	}
	void Engine::UpdateGame(float deltaTime)
	{
		PROFILER_FUNC("Engine Tick");

		// Pause & skip frame controls.
		if (m_paused && !m_shouldSkipFrame) return;
		m_shouldSkipFrame = false;

		m_eventSystem.Trigger<Event::EPreTick>(Event::EPreTick{ (float)m_rawDeltaTime, m_isInPlayMode });

		// Physics events & physics tick.
		m_physicsAccumulator += deltaTime;
		if (m_physicsAccumulator >= PHYSICS_STEP)
		{
			m_physicsAccumulator -= PHYSICS_STEP;
			m_eventSystem.Trigger<Event::EPrePhysicsTick>(Event::EPrePhysicsTick{});
			m_physicsEngine.Tick(PHYSICS_STEP);
			m_eventSystem.Trigger<Event::EPhysicsTick>(Event::EPhysicsTick{ PHYSICS_STEP, m_isInPlayMode });
			m_eventSystem.Trigger<Event::EPrePhysicsTick>(Event::EPrePhysicsTick{ PHYSICS_STEP, m_isInPlayMode });
		}

		// Other main systems (engine or game)
		m_mainECSPipeline.UpdateSystems(deltaTime);

		// Animation, particle systems.
		m_renderEngine.Tick(deltaTime);

		m_eventSystem.Trigger<Event::ETick>(Event::ETick{ (float)m_rawDeltaTime, m_isInPlayMode });
		m_eventSystem.Trigger<Event::EPostTick>(Event::EPostTick{ (float)m_rawDeltaTime, m_isInPlayMode });
	}

	void Engine::DisplayGame(float interpolation)
	{
		PROFILER_FUNC("Engine Render");

		if (m_canRender)
		{
			m_renderEngine.Render(interpolation);
			m_window.Tick();
		}
	}
	void Engine::RemoveOutliers(bool biggest)
	{
		double outlier = biggest ? 0 : 10;
		int outlierIndex = -1;
		int indexCounter = 0;
		for (double d : m_deltaTimeArray)
		{
			if (d < 0)
			{
				indexCounter++;
				continue;
			}

			if (biggest)
			{
				if (d > outlier)
				{
					outlierIndex = indexCounter;
					outlier = d;
				}
			}
			else
			{
				if (d < outlier)
				{
					outlierIndex = indexCounter;
					outlier = d;
				}
			}

			indexCounter++;
		}

		if (outlierIndex != -1)
			m_deltaTimeArray[outlierIndex] = m_deltaTimeArray[outlierIndex] * -1.0;
	}
	double Engine::SmoothDeltaTime(double dt)
	{

		if (m_deltaFirstFill < DELTA_TIME_HISTORY)
		{
			m_deltaFirstFill++;
		}
		else if (!m_deltaFilled)
			m_deltaFilled = true;

		m_deltaTimeArray[m_deltaTimeArrOffset] = dt;
		m_deltaTimeArrOffset++;


		if (m_deltaTimeArrOffset == DELTA_TIME_HISTORY)
			m_deltaTimeArrOffset = 0;

		if (!m_deltaFilled)
			return dt;

		// Remove the biggest & smalles 2 deltas.
		RemoveOutliers(true);
		RemoveOutliers(true);
		RemoveOutliers(false);
		RemoveOutliers(false);

		double avg = 0.0;
		int index = 0;
		for (double d : m_deltaTimeArray)
		{
			if (d < 0.0)
			{
				m_deltaTimeArray[index] = m_deltaTimeArray[index] * -1.0;
				index++;
				continue;
			}

			avg += d;
			index++;
		}

		avg /= DELTA_TIME_HISTORY - 4;

		return avg;
	}

}