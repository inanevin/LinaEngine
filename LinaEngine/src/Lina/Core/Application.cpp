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
//#include "Lina/PackageManager/PAMWindow.hpp"

namespace LinaEngine
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::instance = nullptr;

	Application::Application()
	{
		LINA_CORE_TRACE("[Constructor] -> Application ({0})", typeid(*this).name());
		LINA_CORE_ASSERT(!instance, "Application already exists!");

		// Set singleton instance.
		instance = this;

		// Set unique pointers of input device & render engine.
		m_InputEngine = std::make_unique<PAMInputEngine>();
		m_RenderEngine = std::make_unique<PAMRenderEngine>();

		// Create main window.
		bool windowCreationSuccess = m_RenderEngine->CreateContextWindow();
		if (!windowCreationSuccess)
		{
			LINA_CORE_ERR("Window Creation Failed!");
			return;
		}

		// Set event callback for main window.
		m_RenderEngine->SetMainWindowEventCallback(BIND_EVENT_FN(OnEvent));

		// Initialize input & render engines.
		m_InputEngine->Initialize(m_RenderEngine->GetNativeWindow());
		m_RenderEngine->Initialize();

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

		//m_RenderDevice->OnEvent(e);
		
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
			// Update engines.
			m_InputEngine->Tick();
			m_RenderEngine->Tick();

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

	/*RenderDevice * Application::GetRenderDevice()
	{
		LINA_CORE_ASSERT(m_RenderDevice != nullptr, "Null render device get call.");
		return m_RenderDevice;
	}

	InputEngine * Application::GetInputDevice()
	{
		LINA_CORE_ASSERT(m_RenderDevice != nullptr, "Null input device get call.");
		return m_InputEngine;
	}*/

}

