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


#include "Events/ApplicationEvent.hpp"
#include "LayerStack.hpp"
#include "Input/InputEngine.hpp"
#include "Rendering/RenderEngine.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "Physics/PhysicsEngine.hpp"

namespace LinaEngine::World
{
	class Level;
}

namespace LinaEngine
{


	class Application
	{
	public:

		virtual ~Application();

		// Main application loop.
		void Run();

		// Pushes a new layer into the application stack.
		void PushLayer(Layer* layer);

		// Pushes a new overlay layer into the application stack.
		void PushOverlay(Layer* layer);

		// Loads a level into memory.
		void LoadLevel(LinaEngine::World::Level* level);

	protected:

		Application();

		FORCEINLINE LinaEngine::Graphics::RenderEngine& GetRenderEngine() { return m_RenderEngine; }

	private:

		// Called when an internal event occurs.
		void OnEvent(Event& e);

		// Called when main application window is closed.
		bool OnWindowClose(WindowCloseEvent& e);

	private:

		// Layer queue.
		LayerStack m_LayerStack;

		// Active engines running in the application.
		LinaEngine::Input::InputEngine m_InputEngine;
		LinaEngine::Graphics::RenderEngine m_RenderEngine;
		LinaEngine::Physics::PhysicsEngine m_PhysicsEngine;
		LinaEngine::ECS::EntityComponentSystem m_ECS;

		// Current active level.
		LinaEngine::World::Level* m_CurrentLevel = nullptr;

		// Do we have a currently active level?
		bool m_ActiveLevelExists = false;

		// Is application running?
		bool m_Running = false;

	};

	// Defined in client.
	Application* CreateApplication();


}


#endif