/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Lina_Application
Timestamp: 12/29/2018 10:43:46 PM

*/

#include "LinaPch.hpp"
#include "Core/Application.hpp"
#include "Core/Layer.hpp"
#include "World/Level.hpp"

namespace LinaEngine
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)


	Application::Application()
	{
		LINA_CORE_TRACE("[Constructor] -> Application ({0})" , typeid(*this).name());
		LINA_CORE_ASSERT(!instance, "Application already exists!");

		// Create main window.
		bool windowCreationSuccess = m_RenderEngine.CreateContextWindow(m_InputEngine);
		if (!windowCreationSuccess)
		{
			LINA_CORE_ERR("Window Creation Failed!");
			return;
		}

		// Set event callback for main window.
		m_RenderEngine.SetMainWindowEventCallback(BIND_EVENT_FN(OnEvent));

		// Initialize engines.
		m_InputEngine.Initialize(m_RenderEngine.GetNativeWindow());
		m_PhysicsEngine.Initialize(m_ECS);
		m_RenderEngine.Initialize(m_ECS);

		// Set running flag.
		m_Running = true;
	}

	Application::~Application()
	{
		LINA_CORE_TRACE("[Destructor] -> Application ({0})", typeid(*this).name());
	}

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		// Propagate the window resize event to render engine.
		if (e.GetEventType() == EventType::WindowResize)
		{
			WindowResizeEvent& windowEvent = (WindowResizeEvent&)(e);
			m_RenderEngine.OnWindowResized((float)windowEvent.GetWidth(), (float)windowEvent.GetHeight());
		}
		
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.isHandled)
				break;
		}
	}

	void Application::Run()
	{

		while (m_Running)
		{
			// Update input engine.
			m_InputEngine.Tick();
	
			// Update physics engine.
			m_PhysicsEngine.Tick(0.01f);

			// Update render engine.
			m_RenderEngine.Tick(0.01f);

			// Update current level.
			if (m_ActiveLevelExists)
				m_CurrentLevel->Tick(0.01f);
	
			// Update layers.
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();
		}

	}

	bool Application::OnWindowClose(WindowCloseEvent&e)
	{
		m_Running = false;
		return true;
	}

	void Application::PushLayer(Layer * layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer * layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	 void Application::LoadLevel(LinaEngine::World::Level * level)
	{
		// TODO: Implement unloading the current level & loading a new one later.
		m_CurrentLevel = level;
		m_CurrentLevel->SetEngineReferences(m_ECS, m_RenderEngine, m_InputEngine);
		m_CurrentLevel->Install();
		m_CurrentLevel->Initialize();
		m_ActiveLevelExists = true;
	}
}

