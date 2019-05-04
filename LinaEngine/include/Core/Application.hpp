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
#include "PackageManager/PAMInputEngine.hpp"
#include "Rendering/RenderEngine.hpp"
#include "LayerStack.hpp"
#include "Common.hpp"
#include "APIExport.hpp"
#include "ECS/EntityComponentSystem.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "World/Level.hpp"

namespace LinaEngine
{
	using namespace Graphics;
	using namespace Input;
	using namespace ECS;
	using namespace Physics;
	using namespace World;

	class Application
	{
	public:

		LINA_API Application();
		LINA_API virtual ~Application();

		// Initialize the application
		LINA_API virtual void Initialize();

		// Main application loop.
		LINA_API void Run();

		// Called when an internal event occurs.
		LINA_API void OnEvent(Event& e);

		// Pushes a new layer into the application stack.
		LINA_API void PushLayer(Layer* layer);

		// Pushes a new overlay layer into the application stack.
		LINA_API void PushOverlay(Layer* layer);

		// Loads a level into memory.
		LINA_API void LoadLevel(Level* level);

		// Called when main application window is closed.
		bool OnWindowClose(WindowCloseEvent& e);

		// Return the static application instance.
		FORCEINLINE static Application& Get() { return *instance; }
		FORCEINLINE InputEngine<PAMInputEngine>& GetInputDevice() { return *(m_InputEngine.get()); }
		FORCEINLINE RenderEngine& GetRenderEngine() { return *(m_RenderEngine.get()); }

		
	private:

		static Application* instance;

		bool m_Running = false;

		LayerStack m_LayerStack;

		// Do we have a currently active level?
		bool m_ActiveLevelExists = false;

		// Current active level.
		Level* m_CurrentLevel = nullptr;

		// Active engines running in the application.
		std::unique_ptr<InputEngine<PAMInputEngine>> m_InputEngine;
		std::unique_ptr<RenderEngine> m_RenderEngine;
		std::unique_ptr<PhysicsEngine> m_PhysicsEngine;
		std::unique_ptr<EntityComponentSystem> m_ECS;

	};

	// Defined in client.
	Application* CreateApplication();

	
}


#endif