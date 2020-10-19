/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
Class: Application

Central application class, responsible for managing all the engines like input, physics, rendering etc.
as well as defining the game loop.

Timestamp: 12/29/2018 10:43:46 PM
*/

#pragma once
#ifndef Lina_Application_HPP
#define Lina_Application_HPP

#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Color.hpp"
#include "Core/LayerStack.hpp"
#include "ECS/ECSSystem.hpp"
#include "Actions/ActionDispatcher.hpp"
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

		void PushLayer(Layer& layer);
		void PushOverlay(Layer& layer);

		// Loads a level into memory.
		bool InstallLevel(LinaEngine::World::Level& level);

		void InitializeLevel(LinaEngine::World::Level& level);

		// Unloads a level from memory.
		void UninstallLevel(LinaEngine::World::Level& level);

		int GetCurrentFPS() { return m_currentFPS; }
		bool GetActiveLevelExists() { return m_activeLevelExists; }

		static Action::ActionDispatcher& GetEngineDispatcher() { return s_engineDispatcher; }
		static Application& GetApp() { return *s_application; }
		static Graphics::Window& GetAppWindow() { return *s_appWindow; }
		static Graphics::RenderEngine& GetRenderEngine() { return *s_renderEngine; }
		static Input::InputEngine& GetInputEngine() { return *s_inputEngine; }
		static Physics::PhysicsEngine& GetPhysicsEngine() { return *s_physicsEngine; }
		static ECS::ECSRegistry& GetECSRegistry() { return s_ecs; }

	protected:

		virtual void Initialize(Graphics::WindowProperties& props);

		Application();

		// Delegates draw commands from physics engine to rendering engine
		virtual void OnDrawLine(Vector3 from, Vector3 to, Color color, float width = 1.0f);


	private:

		// Callbacks.
		void OnLog(Log::LogDump dump);
		bool OnWindowClose();
		void OnWindowResize(Vector2 size);
		void OnPostSceneDraw();
		void KeyCallback(int key, int action);
		void MouseCallback(int button, int action);
		void WindowCloseCallback() {};
		

	private:

		static Action::ActionDispatcher s_engineDispatcher;

		// Layer queue.
		LayerStack m_layerStack;

		// Active engines running in the application.
		static Application* s_application;
		static Input::InputEngine* s_inputEngine;
		static Graphics::RenderEngine* s_renderEngine;
		static Physics::PhysicsEngine* s_physicsEngine;
		static ECS::ECSRegistry s_ecs;
		static Graphics::Window* s_appWindow;

		Input::InputDevice* m_inputDevice = nullptr;
		World::Level* m_currentLevel = nullptr;

		bool m_activeLevelExists = false;
		bool m_running = false;
		bool m_firstRun = true;
		bool m_canRender = true;

		// Callbacks
		std::function<void(int, int)> m_keyCallback;
		std::function<void(int, int)> m_mouseCallback;
		std::function<void(Vector2)> m_WwndowResizeCallback;
		std::function<void()> m_windowClosedCallback;
		std::function<void(Vector3, Vector3, Color, float)> m_drawLineCallback;
		std::function<void()> m_postSceneDrawCallback;

		int m_currentFPS = 0;
		double m_previousTime = 0;
		int m_fpsCounter = 0;
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