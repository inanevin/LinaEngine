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

namespace LinaEngine
{
	Action::ActionDispatcher Application::s_engineDispatcher;

	Application::Application()
	{
		s_engineDispatcher.Initialize(Action::ActionType::EngineActionsStartIndex, Action::ActionType::EngineActionsEndIndex);

		// Make sure log event is delegated to the application.
		Log::s_onLog = std::bind(&Application::OnLog, this, std::placeholders::_1);

		LINA_CORE_TRACE("[Constructor] -> Application ({0})", typeid(*this).name());
		LINA_CORE_ASSERT(!instance, "Application already exists!");
	}

	void Application::Initialize(Graphics::WindowProperties& props)
	{
		// Get engine instances.
		m_appWindow = CreateContextWindow();
		m_inputDevice = CreateInputDevice();
		m_renderEngine = CreateRenderEngine();
		m_inputEngine = CreateInputEngine();
		m_physicsEngine = CreatePhysicsEngine();

		// Create main window.
		bool windowCreationSuccess = m_appWindow->CreateContext(props);
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
		m_appWindow->SetKeyCallback(m_keyCallback);
		m_appWindow->SetMouseCallback(m_mouseCallback);
		m_appWindow->SetWindowResizeCallback(m_WwndowResizeCallback);
		m_appWindow->SetWindowClosedCallback(m_windowClosedCallback);
		m_renderEngine->SetPostSceneDrawCallback(m_postSceneDrawCallback);
		m_renderEngine->SetViewportDisplay(Vector2::Zero, m_appWindow->GetSize());

		m_inputEngine->Initialize(m_appWindow->GetNativeWindow(), m_inputDevice);
		m_physicsEngine->Initialize(m_ecs, m_drawLineCallback);
		m_renderEngine->Initialize(m_ecs, *m_appWindow);

		m_running = true;
	}


	Application::~Application()
	{
		if (m_physicsEngine)
			delete m_physicsEngine;

		if (m_inputEngine)
			delete m_inputEngine;

		if (m_renderEngine)
			delete m_renderEngine;

		if (m_inputDevice)
			delete m_inputDevice;

		if (m_appWindow)
			delete m_appWindow;

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
		double currentTime = (double)m_appWindow->GetTime();
		double accumulator = 0.0;

		while (m_running)
		{
			// Update input engine.
			m_inputEngine->Tick();

			double newTime = (double)m_appWindow->GetTime();
			double frameTime = newTime - currentTime;

			// Update layers.
			for (Layer* layer : m_layerStack)
				layer->OnTick(frameTime);

			// Update current level.
			if (m_activeLevelExists)
				m_currentLevel->Tick(frameTime);

			if (frameTime > 0.25)
				frameTime = 0.25;

			currentTime = newTime;
			accumulator += frameTime;

			while (accumulator >= dt)
			{
				m_physicsEngine->Tick(dt);
				t += dt;
				accumulator -= dt;
			}

			if (m_canRender)
			{
				// render level.
				if (m_activeLevelExists)
					m_renderEngine->Render();

				// Update gui layers & swap buffers
				m_renderEngine->TickAndSwap(frameTime);
			}

			// Simple FPS count
			m_fpsCounter++;

			if (currentTime - m_previousTime >= 1.0)
			{
				m_previousTime = currentTime;
				m_currentFPS = m_fpsCounter;
				m_fpsCounter = 0;
			}

			if (m_firstRun)
				m_firstRun = false;

		}
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

		m_renderEngine->SetViewportDisplay(Vector2::Zero, size);

		s_engineDispatcher.DispatchAction<Vector2>(Action::ActionType::WindowResized, size);
	}

	void Application::OnPostSceneDraw()
	{
		m_physicsEngine->OnPostSceneDraw();

		s_engineDispatcher.DispatchAction<void*>(Action::ActionType::PostSceneDraw, 0);
	}

	void Application::KeyCallback(int key, int action)
	{
		m_inputEngine->DispatchKeyAction(static_cast<LinaEngine::Input::InputCode::Key>(key), action);
	}

	void Application::MouseCallback(int button, int action)
	{
		m_inputEngine->DispatchMouseAction(static_cast<LinaEngine::Input::InputCode::Mouse>(button), action);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_layerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	bool Application::InstallLevel(LinaEngine::World::Level* level)
	{
		level->SetEngineReferences(&m_ecs, *m_renderEngine, *m_inputEngine);
		return level->Install();
	}

	void Application::InitializeLevel(LinaEngine::World::Level* level)
	{
		m_currentLevel = level;
		m_currentLevel->Initialize();
		m_activeLevelExists = true;
	}


	void Application::UnloadLevel(LinaEngine::World::Level* level)
	{
		if (m_currentLevel == level)
		{
			m_activeLevelExists = false;
			m_currentLevel = nullptr;
		}

		level->Uninstall();
	}

	void Application::OnDrawLine(Vector3 from, Vector3 to, Color color, float width)
	{
		m_renderEngine->DrawLine(from, to, color, width);
	}
}

