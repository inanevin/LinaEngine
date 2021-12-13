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

#include "EventSystem/Events.hpp"
#include "Core/InputBackend.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Core/PhysicsBackend.hpp"
#include "Core/AudioEngine.hpp"
#include "Core/WindowBackend.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Math/Vector.hpp"
#include "Math/Color.hpp"
#include "ECS/ECS.hpp"
#include <functional>
#include <array>

namespace Lina::World
{
	class Level;
}


namespace Lina::Audio
{
	class AudioEngine;
}

namespace Lina
{

	class Application
	{
	public:

#define DELTA_TIME_HISTORY 11

		virtual ~Application();

		// Main application loop.
		void Run();



		// Loads a level into memory.
		bool InstallLevel(Lina::World::Level& level, bool loadFromFile = false, const std::string& path = "", const std::string& levelName = "");
		void SaveLevelData(const std::string& folderPath, const std::string& fileName);
		void LoadLevelData(const std::string& folderPath, const std::string& fileName);
		void RestartLevel();

		// Unloads a level from memory.
		void UninstallLevel();

		int GetCurrentFPS() { return m_currentFPS; }
		int GetCurrentUPS() { return m_currentUPS; }
		bool GetActiveLevelExists() { return m_activeLevelExists; }
		double GetRawDelta() { return m_rawDeltaTime; }
		double GetSmoothDelta() { return m_smoothDeltaTime; }
		double GetFrameTime() { return m_frameTime; }
		double GetRenderTime() { return m_renderTime; }
		double GetUpdateTime() { return m_updateTime; }
		void AddToMainPipeline(ECS::BaseECSSystem& system) { m_mainECSPipeline.AddSystem(system); }
		void SetPlayMode(bool enabled);
		bool GetPlayMode() { return m_isInPlayMode; }
		World::Level* GetCurrentLevel() { return m_currentLevel; }

		double GetTime();
		static Application& GetApp() { return *s_application; }

	protected:

		virtual void Initialize(WindowProperties& props);

		Application();

	private:

		// Callbacks.
		void UpdateGame(float deltaTime);
		void DisplayGame(float interpolation);
		void OnLog(Event::ELog dump);
		bool OnWindowClose(Event::EWindowClosed);
		void OnWindowResize(Event::EWindowResized);

		void RemoveOutliers(bool biggest);
		double SmoothDeltaTime(double dt);
		void InitializeLevel(Lina::World::Level& level);

	private:

		// Active engines running in the application.
		static Application* s_application;
		Graphics::RenderEngineBackend m_renderEngine;
		Physics::PhysicsEngineBackend m_physicsEngine;
		Audio::AudioEngine m_audioEngine;
		Input::InputEngineBackend m_inputEngine;
		Graphics::WindowBackend m_window;
		Event::EventSystem m_eventSystem;
		ECS::Registry m_ecs;

		World::Level* m_currentLevel = nullptr;
		ECS::ECSSystemList m_mainECSPipeline;

		bool m_activeLevelExists = false;
		bool m_running = false;
		bool m_firstRun = true;
		bool m_canRender = true;
		bool m_isInPlayMode = false;

		// Performance & variable stepping
		int m_currentFPS = 0;
		int m_currentUPS = 0;
		double m_updateTime = 0;
		double m_renderTime = 0;
		double m_frameTime = 0;
		double m_smoothDeltaTime;
		double m_rawDeltaTime;
		std::array<double, DELTA_TIME_HISTORY> m_deltaTimeArray;
		uint8 m_deltaTimeArrIndex = 0;
		uint8 m_deltaTimeArrOffset = 0;
		int m_deltaFirstFill = 0;
		bool m_deltaFilled = false;
	};

	// Defined in client.
	Application* CreateApplication();
	
};


#endif