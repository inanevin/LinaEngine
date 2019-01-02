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
//#include "glew.h"

namespace LinaEngine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application()
	{
		m_Running = true;

		// Init graphics adapter.
		GraphicsAdapter* newAdapter = new GraphicsAdapter();

		// Get Window.
		m_Window = std::unique_ptr<Window>(newAdapter->CreateEngineWindow());
	
		// Set window callbacks.
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

	}

	Application::~Application()
	{

	}

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		LINA_CORE_TRACE("{0}", e);

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
			

			m_Window->OnUpdate();

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

