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

#pragma once
#ifndef Lina_Application_HPP
#define Lina_Application_HPP

#include "Core.hpp"
#include "Lina/Events/ApplicationEvent.hpp"
#include "LayerStack.hpp"

namespace LinaEngine
{
	class RenderingEngine;
	class InputEngine;

	class LINA_API Application
	{
	public:

		Application();
		virtual ~Application();
		void Run();
	    void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

	private:

		
		bool m_Running = false;
		LayerStack m_LayerStack;
		std::unique_ptr<RenderingEngine> m_RenderingEngine;
		std::unique_ptr<InputEngine> m_InputEngine;
	};

	// Defined in client.
	Application* CreateApplication();
}


#endif