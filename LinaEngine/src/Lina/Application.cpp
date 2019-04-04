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
#include "Application.hpp"  
#include "Rendering/RenderingEngine.hpp"
#include "Input/InputEngine.hpp"
#include "PackageManager/Graphics/GraphicsAdapter.hpp"
#include "PackageManager/Input/InputAdapter.hpp"

namespace LinaEngine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::instance = nullptr;

	Application::Application()
	{
		LINA_CORE_ASSERT(!instance, "Application already exists!");

		// Set singleton instance.
		instance = this;

		// Get a graphics adapter.
		GraphicsAdapter adpt;

		// Create unique ptr for rendering engine returned by the adapter.
		m_RenderingEngine = std::unique_ptr<RenderingEngine>(adpt.CreateRenderingEngine());

		// Set event callback for the main window.
		m_RenderingEngine->GetMainWindow().SetEventCallback(BIND_EVENT_FN(OnEvent));

		// Set application reference.
		m_RenderingEngine->SetApplication(*this);

		// Get an input adapter.
		InputAdapter inpAdpt;

		// Create input engine.
		m_InputEngine = std::unique_ptr<InputEngine>(inpAdpt.CreateInputEngine());

		// Set window reference in input engine.
		m_InputEngine->SetApplication(*this);

		// Set running flag.
		m_Running = true;

		// Initialize rendering engine.
		m_RenderingEngine->Initialize();

		// Initialize input engine.
		m_InputEngine->Initialize();

		test = new ActionTest();
		test->SetAction();
	}

	Application::~Application()
	{

	}

	void Application::OnEvent(Event & e)
	{
	
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
	

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
			
			m_InputEngine->OnUpdate();

			// Update rendering engine.
			m_RenderingEngine->OnUpdate();


			std::cout << m_InputEngine->GetMouseAxis().ToString() << std::endl;

			if (m_InputEngine->GetKeyDown(LINA_KEY_K))
				delete test;

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

}

