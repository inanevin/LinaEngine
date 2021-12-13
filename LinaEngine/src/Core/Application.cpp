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

#include "LinaPch.hpp"
#include "Core/Application.hpp"

#include "Core/Layer.hpp"
#include "World/DefaultLevel.hpp"
#include "Core/Timer.hpp"

#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"

#include <chrono>

namespace Lina
{
#define PHYSICS_DELTA 0.01666

	Lina::Application* Application::s_application = nullptr;

	Application::Application()
	{
		s_application = this;
		Log::s_onLogSink.connect<&Application::OnLog>(this);

		Event::EventSystem::s_eventSystem = &m_eventSystem;
		ECS::Registry::s_ecs = &m_ecs;
		Graphics::WindowBackend::s_openglWindow = &m_window;
		Graphics::RenderEngineBackend::s_renderEngine = &m_renderEngine;
		Physics::PhysicsEngineBackend::s_physicsEngine = &m_physicsEngine;
		Input::InputEngineBackend::s_inputEngine = &m_inputEngine;

		m_eventSystem.Connect<Event::EWindowClosed, &Application::OnWindowClose>(this);
		m_eventSystem.Connect<Event::EWindowResized, &Application::OnWindowResize>(this);

		LINA_TRACE("[Constructor] -> Application ({0})", typeid(*this).name());
	}

	void Application::Initialize(WindowProperties& props)
	{
		m_inputEngine.Initialize();

		// Build main window.
		bool windowCreationSuccess = m_window.CreateContext(props);
		if (!windowCreationSuccess)
		{
			LINA_ERR("Window Creation Failed!");
			return;
		}

		// Set event callback for main window.
		m_renderEngine.SetViewportDisplay(Vector2::Zero, m_window.GetSize());

		m_ecs.Initialize();
		m_physicsEngine.Initialize(m_drawLineCallback);
		m_renderEngine.Initialize();
		m_audioEngine.Initialize();


		// Register ECS components for cloning & serialization functionality.
		m_ecs.RegisterComponent<ECS::EntityDataComponent>();
		m_ecs.RegisterComponent<ECS::FreeLookComponent>();
		m_ecs.RegisterComponent<ECS::RigidbodyComponent>();
		m_ecs.RegisterComponent<ECS::CameraComponent>();
		m_ecs.RegisterComponent<ECS::PointLightComponent>();
		m_ecs.RegisterComponent<ECS::SpotLightComponent>();
		m_ecs.RegisterComponent<ECS::DirectionalLightComponent>();
		m_ecs.RegisterComponent<ECS::MeshRendererComponent>();
		m_ecs.RegisterComponent<ECS::ModelRendererComponent>();
		m_ecs.RegisterComponent<ECS::SpriteRendererComponent>();

		m_deltaTimeArray.fill(-1.0);
		m_isInPlayMode = true;
		m_running = true;
	}


	Application::~Application()
	{
		LINA_TRACE("[Destructor] -> Application ({0})", typeid(*this).name());
	}


	void Application::Run()
	{
		int frames = 0;
		int updates = 0;
		double totalFPSTime = GetTime();
		double previousFrameTime;
		double currentFrameTime = GetTime();

		while (m_running)
		{

			previousFrameTime = currentFrameTime;
			currentFrameTime = GetTime();

			m_rawDeltaTime = (currentFrameTime - previousFrameTime);
			m_smoothDeltaTime = SmoothDeltaTime(m_rawDeltaTime);
			m_eventSystem.Trigger<Event::ETick>(Event::ETick{ (float)m_rawDeltaTime, m_isInPlayMode });

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

			double now = GetTime();
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

		Log::s_onLogSink.disconnect(this);
		Timer::UnloadTimers();
	}


	void Application::UpdateGame(float deltaTime)
	{
		// Tick physics (fixed)
		m_physicsEngine.Tick(0.02);

		// Main layers.
		for (Layer* layer : m_mainLayerStack)
			layer->Tick(deltaTime);

		// Game layers.
		if (m_isInPlayMode)
		{
			for (Layer* layer : m_playModeStack)
				layer->Tick(deltaTime);
		}

		// Level
		if (m_activeLevelExists)
			m_currentLevel->Tick(m_isInPlayMode, deltaTime);

		// Other main systems (engine or game)
		m_mainECSPipeline.UpdateSystems(deltaTime);

		// Animation, particle systems.
		m_renderEngine.Tick(deltaTime);


		// Main layers post.
		for (Layer* layer : m_mainLayerStack)
			layer->PostTick(deltaTime);

		// Game layers post.
		if (m_isInPlayMode)
		{
			for (Layer* layer : m_playModeStack)
				layer->PostTick(deltaTime);
		}

		// Level post.
		if (m_activeLevelExists)
			m_currentLevel->PostTick(m_isInPlayMode, deltaTime);
	}

	void Application::DisplayGame(float interpolation)
	{
		if (m_canRender)
		{
			m_eventSystem.Trigger<Event::EPreRender>(Event::EPreRender{});

			// render level.
			if (m_activeLevelExists)
				m_renderEngine.Render(interpolation);

			m_eventSystem.Trigger<Event::EPostRender>(Event::EPostRender{});
			m_eventSystem.Trigger<Event::EFinalizePostRender>(Event::EFinalizePostRender{});

			m_renderEngine.RenderLayers();
			m_window.Tick();
		}
	}

	void Application::OnLog(Event::ELog dump)
	{
		std::string msg = dump.m_message;

		// if (dump.m_level == Lina::LogLevel::Error)
		// 	msg = "\033{1;31m" + dump.m_message + "\033[0m";
		// else if (dump.m_level == Lina::LogLevel::Warn)
		// 	msg = "\033{1;33m" + dump.m_message + "\033[0m";

		std::cout << msg << std::endl;

		m_eventSystem.Trigger<Event::ELog>(dump);
	}

	bool Application::OnWindowClose(Event::EWindowClosed event)
	{
		m_running = false;
		return true;
	}

	void Application::OnWindowResize(Event::EWindowResized event)
	{
		if (event.m_windowProps.m_width == 0.0f || event.m_windowProps.m_height == 0.0f)
			m_canRender = false;
		else
			m_canRender = true;

	}


	void Application::RemoveOutliers(bool biggest)
	{
		int outlier = biggest ? 0 : 10;
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

	double Application::SmoothDeltaTime(double dt)
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



	void Application::SetPlayMode(bool enabled)
	{
		m_isInPlayMode = enabled;
		m_eventSystem.Trigger<Event::EPlayModeChanged>(Event::EPlayModeChanged{ enabled });
	}

	bool Application::InstallLevel(Lina::World::Level& level, bool loadFromFile, const std::string& path, const std::string& levelName)
	{
		UninstallLevel();

		bool install = level.Install(loadFromFile, path, levelName);
		m_currentLevel = &level;
		m_eventSystem.Trigger<Event::ELevelInstalled>(Event::ELevelInstalled{});
		InitializeLevel(level);
		return install;
	}

	void Application::InitializeLevel(Lina::World::Level& level)
	{
		m_currentLevel->Initialize();
		m_eventSystem.Trigger<Event::ELevelInitialized>(Event::ELevelInitialized{});
		m_activeLevelExists = true;
	}

	void Application::SaveLevelData(const std::string& folderPath, const std::string& fileName)
	{
		if (m_currentLevel != nullptr)
			m_currentLevel->SerializeLevelData(folderPath, fileName);
	}

	void Application::LoadLevelData(const std::string& folderPath, const std::string& fileName)
	{
		if (m_currentLevel != nullptr)
		{
			InstallLevel(*m_currentLevel, true, folderPath, fileName);
		}
	}

	void Application::RestartLevel()
	{
		InstallLevel(*m_currentLevel);
	}

	void Application::UninstallLevel()
	{
		if (m_currentLevel != nullptr)
		{
			m_currentLevel->Uninstall();
			m_ecs.each([this](auto entity)
				{
					m_ecs.DestroyEntity(entity);
				});
			m_ecs.clear();
			m_eventSystem.Trigger<Event::ELevelUninstalled>(Event::ELevelUninstalled{});
			m_currentLevel = nullptr;
		}
	}

	double Application::GetTime()
	{
		return m_window.GetTime();
	}

}

