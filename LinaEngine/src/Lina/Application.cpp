/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Lina_Application
Timestamp: 12/29/2018 10:43:46 PM

*/

#include "LinaPch.hpp"
#include "Application.hpp"  
#include "RenderingEngine.hpp"
#include "Package Manager/GraphicsAdapter.hpp"

namespace LinaEngine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application()
	{
		// Get a graphics adapter.
		GraphicsAdapter adpt;

		// Create unique ptr for rendering engine returned by the adapter.
		m_RenderingEngine = std::unique_ptr<RenderingEngine>(adpt.CreateRenderingEngine());

		// Set event callback for the main window.
		m_RenderingEngine->GetMainWindow().SetEventCallback(BIND_EVENT_FN(OnEventFromWindow));

		// Set running flag.
		m_Running = true;
	}

	Application::~Application()
	{
		
	}

	void Application::OnEventFromWindow(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		LINA_CORE_INFO(e);

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
			// Update rendering engine.
			m_RenderingEngine->OnUpdate();

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
	}
	void Application::PushOverlay(Layer * layer)
	{
		m_LayerStack.PushOverlay(layer);
	}
}

