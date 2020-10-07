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

#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Color.hpp"
#include "Core/LayerStack.hpp"
#include "ECS/ECSSystem.hpp"

#include <functional>

namespace LinaEngine::World
{
	class Level;
}

namespace LinaEngine::Graphics
{
	class Window;
	class RenderEngine;
	struct WindowProperties;
}

namespace LinaEngine::Input
{
	class InputEngine;
	class InputDevice;
}

namespace LinaEngine::Physics
{
	class PhysicsEngine;
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

		Application(Graphics::WindowProperties& props);

		// Delegates draw commands from physics engine to rendering engine
		virtual void OnDrawLine(Vector3 from, Vector3 to, Color color, float width = 1.0f);

		// Get render engine
		FORCEINLINE Graphics::Window& GetAppWindow() { return *m_appWindow; }
		FORCEINLINE Graphics::RenderEngine& GetRenderEngine() { return *m_renderEngine; }
		FORCEINLINE Input::InputEngine& GetInputEngine() { return *m_inputEngine; }
		FORCEINLINE Physics::PhysicsEngine& GetPhysicsEngine() { return *m_physicsEngine; }
		FORCEINLINE ECS::ECSRegistry& GetECSREgistry() { return m_ECS; }

	private:


		// Called when an internal event occurs.
		void OnEvent();

		// Called when main application window is closed.
		bool OnWindowClose();

		// Window resize event
		void OnWindowResize(Vector2 size);

		// Called when scene objects are drawn
		void OnPostSceneDraw();

		// Callbacks & events.
		void KeyCallback(int key, int action);
		void MouseCallback(int button, int action);
		void WindowCloseCallback() {};
		

	private:

		// Layer queue.
		LayerStack m_LayerStack;

		// Active engines running in the application.
		Input::InputEngine* m_inputEngine;
		Graphics::RenderEngine* m_renderEngine;
		Physics::PhysicsEngine* m_physicsEngine;
		ECS::ECSRegistry m_ECS;

		// Devices.
		Graphics::Window* m_appWindow = nullptr;
		Input::InputDevice* m_inputDevice = nullptr;

		// Current active level.
		World::Level* m_CurrentLevel = nullptr;

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
		std::function<void(Vector3, Vector3, Color, float)> m_drawLineCallback;
		std::function<void()> m_postSceneDrawCallback;

		// FPS counting
		int m_CurrentFPS;
		double m_PreviousTime;
		int m_FPSCounter;
	};

	// Defined in client.
	Application* CreateApplication();
	Graphics::Window* CreateContextWindow();
	Graphics::RenderEngine* CreateRenderEngine();
	Input::InputDevice* CreateInputDevice();
	Input::InputEngine* CreateInputEngine();
	Physics::PhysicsEngine* CreatePhysicsEngine();

};


#endif