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
#include "Rendering/RenderEngine.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Input/InputEngine.hpp"
#include "Rendering/Window.hpp"
#include "Core/Layer.hpp"
#include "World/Level.hpp"
#include "Core/Timer.hpp"


namespace LinaEngine
{
	Action::ActionDispatcher Application::s_engineDispatcher;
	Input::InputEngine* Application::s_inputEngine = nullptr;
	Graphics::RenderEngine* Application::s_renderEngine = nullptr;
	Physics::PhysicsEngine* Application::s_physicsEngine = nullptr;
	Graphics::Window* Application::s_appWindow = nullptr;
	ECS::ECSRegistry Application::s_ecs;
	Application* Application::s_application = nullptr;

	Application::Application()
	{
		s_application = this;

		s_engineDispatcher.Initialize(Action::ActionType::EngineActionsStartIndex, Action::ActionType::EngineActionsEndIndex);

		// Make sure log event is delegated to the application.
		Log::s_onLog = std::bind(&Application::OnLog, this, std::placeholders::_1);

		LINA_CORE_TRACE("[Constructor] -> Application ({0})", typeid(*this).name());
		LINA_CORE_ASSERT(!instance, "Application already exists!");
	}

	void Application::Initialize(Graphics::WindowProperties& props)
	{
		// Get engine instances.
		s_appWindow = CreateContextWindow();
		m_inputDevice = CreateInputDevice();
		s_renderEngine = CreateRenderEngine();
		s_inputEngine = CreateInputEngine();
		s_physicsEngine = CreatePhysicsEngine();

		// Create main window.
		bool windowCreationSuccess = s_appWindow->CreateContext(props);
		if (!windowCreationSuccess)
		{
			LINA_CORE_ERR("Window Creation Failed!");
			return;
		}

		// Set callbacks.
		m_keyCallback = std::bind(&Application::KeyCallback, this, std::placeholders::_1, std::placeholders::_2);
		m_mouseCallback = std::bind(&Application::MouseCallback, this, std::placeholders::_1, std::placeholders::_2);
		m_WwndowResizeCallback = std::bind(&Application::OnWindowResize, this, std::placeholders::_1);
		m_windowClosedCallback = std::bind(&Application::OnWindowClose, this);
		m_drawLineCallback = std::bind(&Application::OnDrawLine, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		m_postSceneDrawCallback = std::bind(&Application::OnPostSceneDraw, this);

		// Set event callback for main window.
		s_appWindow->SetKeyCallback(m_keyCallback);
		s_appWindow->SetMouseCallback(m_mouseCallback);
		s_appWindow->SetWindowResizeCallback(m_WwndowResizeCallback);
		s_appWindow->SetWindowClosedCallback(m_windowClosedCallback);
		s_renderEngine->SetPostSceneDrawCallback(m_postSceneDrawCallback);
		s_renderEngine->SetViewportDisplay(Vector2::Zero, s_appWindow->GetSize());

		s_inputEngine->Initialize(s_appWindow->GetNativeWindow(), m_inputDevice);
		s_physicsEngine->Initialize(s_ecs, m_drawLineCallback);
		s_renderEngine->Initialize(s_ecs, *s_appWindow);

		m_running = true;
	}


	Application::~Application()
	{
		if (s_physicsEngine)
			delete s_physicsEngine;

		if (s_inputEngine)
			delete s_inputEngine;

		if (s_renderEngine)
			delete s_renderEngine;

		if (m_inputDevice)
			delete m_inputDevice;

		if (s_appWindow)
			delete s_appWindow;

		LINA_CORE_TRACE("[Destructor] -> Application ({0})", typeid(*this).name());
	}

	void Application::OnLog(Log::LogDump dump)
	{
		// Dump to cout
		std::cout << dump.m_message << std::endl;

		// Dispatch the action to any listener.
		s_engineDispatcher.DispatchAction<Log::LogDump>(Action::ActionType::MessageLogged, dump);
	}

	void Application::Run()
	{
		double t = 0.0;
		double dt = 0.01;
		double currentTime = (double)s_appWindow->GetTime();
		double accumulator = 0.0;
		int fpsCounter = 0;
		double previousTime = 0;

		while (m_running)
		{
			LINA_TIMER_START("Main Loop");

			LINA_TIMER_START("Input Engine Tick");

			// Update input engine.
			s_inputEngine->Tick();

			LINA_TIMER_STOP("Input Engine Tick");

			double newTime = (double)s_appWindow->GetTime();
			double frameTime = newTime - currentTime;

			LINA_TIMER_START("Application Layers Tick");
			
			// Update layers.
			for (Layer* layer : m_layerStack)
				layer->Tick(frameTime);

			LINA_TIMER_STOP("Application Layers Tick");

			LINA_TIMER_START("Current Level Tick");

			// Update current level.
			if (m_activeLevelExists)
				m_currentLevel->Tick(frameTime);

			LINA_TIMER_STOP("Current Level Tick");

			if (frameTime > 0.25)
				frameTime = 0.25;

			currentTime = newTime;
			accumulator += frameTime;

			while (accumulator >= dt)
			{
				LINA_TIMER_START("Physics Tick");
				s_physicsEngine->Tick(dt);
				LINA_TIMER_STOP("Physics Tick");
				t += dt;
				accumulator -= dt;
			}

			LINA_TIMER_START("Render");

			if (m_canRender)
			{
				// render level.
				if (m_activeLevelExists)
					s_renderEngine->Render();
			}

			LINA_TIMER_STOP("Render");

			// Simple FPS count
			fpsCounter++;

			if (currentTime - previousTime >= 1.0)
			{
				previousTime = currentTime;
				m_currentFPS = fpsCounter;
				fpsCounter = 0;
			}

			if (m_firstRun)
				m_firstRun = false;

			LINA_TIMER_STOP("Main Loop");

		}

		Timer::UnloadTimers();
	}

	bool Application::OnWindowClose()
	{
		m_running = false;
		s_engineDispatcher.DispatchAction<void*>(Action::ActionType::WindowClosed, 0);
		return true;
	}

	void Application::OnWindowResize(Vector2 size)
	{
		if (size.x == 0.0f || size.y == 0.0f)
			m_canRender = false;
		else
			m_canRender = true;

		s_renderEngine->SetViewportDisplay(Vector2::Zero, size);

		s_engineDispatcher.DispatchAction<Vector2>(Action::ActionType::WindowResized, size);
	}

	void Application::OnPostSceneDraw()
	{
		s_physicsEngine->OnPostSceneDraw();

		s_engineDispatcher.DispatchAction<void*>(Action::ActionType::PostSceneDraw, 0);
	}

	void Application::KeyCallback(int key, int action)
	{
		s_inputEngine->DispatchKeyAction(static_cast<LinaEngine::Input::InputCode::Key>(key), action);
	}

	void Application::MouseCallback(int button, int action)
	{
		s_inputEngine->DispatchMouseAction(static_cast<LinaEngine::Input::InputCode::Mouse>(button), action);
	}

	void Application::PushLayer(Layer& layer)
	{
		m_layerStack.PushLayer(layer);
	}
	void Application::PushOverlay(Layer& layer)
	{
		m_layerStack.PushOverlay(layer);
	}

	bool Application::InstallLevel(LinaEngine::World::Level& level, bool loadFromFile, const std::string& path, const std::string& levelName)
	{
		if (m_currentLevel != nullptr)
			UninstallLevel(*m_currentLevel);

		bool install = level.Install(loadFromFile, path, levelName);

		s_engineDispatcher.DispatchAction<World::Level*>(Action::ActionType::LevelInstalled, &level);
		return install;
	}

	void Application::InitializeLevel(LinaEngine::World::Level& level)
	{
		m_currentLevel = &level;
		m_currentLevel->Initialize();
		s_engineDispatcher.DispatchAction<World::Level*>(Action::ActionType::LevelInitialized, &level);
		m_activeLevelExists = true;
	}


	void Application::UninstallLevel(LinaEngine::World::Level& level)
	{
		if (m_currentLevel == &level)
		{
			m_activeLevelExists = false;
			m_currentLevel = nullptr;
		}

		level.Uninstall();
		s_ecs.clear();
		s_engineDispatcher.DispatchAction<World::Level*>(Action::ActionType::LevelUninstalled, &level);
	}

	void Application::OnDrawLine(Vector3 from, Vector3 to, Color color, float width)
	{
		s_renderEngine->DrawLine(from, to, color, width);
	}

	double Application::GetTime()
	{
		return s_appWindow->GetTime();
	}

}

