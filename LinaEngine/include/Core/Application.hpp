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


#include "Core/LayerStack.hpp"
#include "Input/InputEngine.hpp"
#include "Rendering/RenderEngine.hpp"
#include "ECS/ECSSystem.hpp"
#include "Physics/PhysicsEngine.hpp"
#include <functional>
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

		// Unloads a level from memory.
		void UnloadLevel(LinaEngine::World::Level* level);

		// Get current frames per second.
		FORCEINLINE int GetCurrentFPS() { return m_CurrentFPS; }

	protected:

		Application();


		// Get render engine
		FORCEINLINE LinaEngine::Graphics::RenderEngine& GetRenderEngine() { return m_RenderEngine; }
		FORCEINLINE LinaEngine::Input::InputEngine& GetInputEngine() { return m_InputEngine; }
		FORCEINLINE LinaEngine::Physics::PhysicsEngine& GetPhysicsngine() { return m_PhysicsEngine; }
		FORCEINLINE LinaEngine::ECS::ECSRegistry& GetECSREgistry() { return m_ECS; }

	private:

		// Called when an internal event occurs.
		void OnEvent();

		// Called when main application window is closed.
		bool OnWindowClose();

		void OnWindowResize(Vector2 size);

		// Callbacks & events.
		FORCEINLINE void KeyCallback(int key, int action) { m_InputEngine.DispatchKeyAction(static_cast<LinaEngine::Input::InputCode::Key>(key), action); }
		FORCEINLINE void MouseCallback(int button, int action) { m_InputEngine.DispatchMouseAction(static_cast<LinaEngine::Input::InputCode::Mouse>(button), action); }
		FORCEINLINE void WindowCloseCallback() {};

	private:

		// Layer queue.
		LayerStack m_LayerStack;

		// Active engines running in the application.
		LinaEngine::Input::InputEngine m_InputEngine;
		LinaEngine::Graphics::RenderEngine m_RenderEngine;
		LinaEngine::Physics::PhysicsEngine m_PhysicsEngine;
		LinaEngine::ECS::ECSRegistry m_ECS;

		// Current active level.
		LinaEngine::World::Level* m_CurrentLevel = nullptr;

		// Do we have a currently active level?
		bool m_ActiveLevelExists = false;

		// Is application running?
		bool m_Running = false;

		// Is his the first run
		bool m_FirstRun = true;

		// Can we render
		bool m_canRender = true;

		// Callbacks
		std::function<void(int, int)> m_KeyCallback;
		std::function<void(int, int)> m_MouseCallback;
		std::function<void(Vector2)> m_WindowResizeCallback;
		std::function<void()> m_WindowClosedCallback;

		// FPS counting
		int m_CurrentFPS;
		double m_PreviousTime;
		int m_FPSCounter;
	};

	// Defined in client.
	Application* CreateApplication();


}


#endif