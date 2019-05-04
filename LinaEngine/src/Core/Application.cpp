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
#include "Core/Time.hpp"

namespace LinaEngine
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::instance = nullptr;

	Application::Application()
	{
		LINA_CORE_TRACE("[Constructor] -> Application ({0})" , typeid(*this).name());
		LINA_CORE_ASSERT(!instance, "Application already exists!");

		// Set singleton instance.
		instance = this;

		// Create unique pointers for engines.
		m_InputEngine = std::make_unique<PAMInputEngine>();
		m_ECS = std::make_unique<EntityComponentSystem>();
		m_RenderEngine = std::make_unique<RenderEngine>();
		m_PhysicsEngine = std::make_unique<PhysicsEngine>();

		// Create main window.
		bool windowCreationSuccess = m_RenderEngine->CreateContextWindow(*m_InputEngine.get());
		if (!windowCreationSuccess)
		{
			LINA_CORE_ERR("Window Creation Failed!");
			return;
		}

		// Set event callback for main window.
		m_RenderEngine->SetMainWindowEventCallback(BIND_EVENT_FN(OnEvent));

		// Initialize engines.
		m_InputEngine->Initialize(m_RenderEngine->GetNativeWindow());
		m_PhysicsEngine->Initialize(m_ECS.get());
		m_RenderEngine->Initialize(m_ECS.get());

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

		if (e.GetEventType() == EventType::WindowResize)
		{
			WindowResizeEvent& windowEvent = (WindowResizeEvent&)(e);
			m_RenderEngine->OnWindowResized(windowEvent.GetWidth(), windowEvent.GetHeight());
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
			m_InputEngine->Tick();
	
			m_PhysicsEngine->Tick(0.01f);
			m_RenderEngine->Tick(0.01f);

			if (m_ActiveLevelExists)
				m_CurrentLevel->Tick(0.01f);

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

	LINA_API void Application::LoadLevel(Level & level)
	{
		// TODO: Implement unloading the current level & loading a new one later.

		level.Install();
		level.Initialize();

		m_CurrentLevel = &level;
		m_ActiveLevelExists = true;
		
	}

	/*RenderDevice * Application::GetRenderDevice()
	{
		LINA_CORE_ASSERT(m_RenderDevice != nullptr, "Null render renderEngine get call.");
		return m_RenderDevice;
	}

	InputEngine * Application::GetInputDevice()
	{
		LINA_CORE_ASSERT(m_RenderDevice != nullptr, "Null input renderEngine get call.");
		return m_InputEngine;
	}*/

}

