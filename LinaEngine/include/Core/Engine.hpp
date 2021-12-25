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
Class: Engine



Timestamp: 12/14/2021 10:09:33 PM
*/

#pragma once

#ifndef Engine_HPP
#define Engine_HPP

// Headers here.
#include "Core/ResourceManager.hpp"
#include "Core/InputBackend.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Core/PhysicsBackend.hpp"
#include "Core/AudioBackend.hpp"
#include "Core/WindowBackend.hpp"
#include "Core/MessageBus.hpp"
#include "EventSystem/EventSystem.hpp"
#include "ECS/SystemList.hpp"
#include "ECS/System.hpp"
#include "ECS/Registry.hpp"

#define DELTA_TIME_HISTORY 11

namespace Lina
{
	class Application;

	class Engine
	{

	public:

		static Engine* Get() { return s_engine; }

		void StartLoadingResources();
		double GetElapsedTime();
		void SetPlayMode(bool enabled);
		void SetIsPaused(bool paused);
		void SkipNextFrame();
		void AddToMainPipeline(ECS::System& system) { m_mainECSPipeline.AddSystem(system); }
		int GetCurrentFPS() { return m_currentFPS; }
		int GetCurrentUPS() { return m_currentUPS; }
		double GetRawDelta() { return m_rawDeltaTime; }
		double GetSmoothDelta() { return m_smoothDeltaTime; }
		double GetFrameTime() { return m_frameTime; }
		double GetRenderTime() { return m_renderTime; }
		double GetUpdateTime() { return m_updateTime; }
		bool GetPlayMode() { return m_isInPlayMode; }
		bool GetPauseMode() { return m_paused; }
	
	private:

		friend class Application;

		Engine() {};
		~Engine() {};

		void Initialize(ApplicationInfo& appInfo);
		void Run();
		void UpdateGame(float deltaTime);
		void DisplayGame(float interpolation);
		void RemoveOutliers(bool biggest);
		double SmoothDeltaTime(double dt);

	private:

		static Engine* s_engine;
		Graphics::RenderEngineBackend m_renderEngine;
		Physics::PhysicsEngineBackend m_physicsEngine;
		Audio::AudioEngineBackend m_audioEngine;
		Input::InputEngineBackend m_inputEngine;
		Graphics::WindowBackend m_window;
		Event::EventSystem m_eventSystem;
		ECS::Registry m_ecs;
		ECS::SystemList m_mainECSPipeline;
		Resources::ResourceManager m_resourceManager;
		MessageBus m_messageBus;

		ApplicationInfo m_appInfo;
		bool m_running = false;
		bool m_canRender = true;
		bool m_firstRun = true;
		bool m_isInPlayMode = false;
		bool m_paused = false;
		bool m_shouldSkipFrame = false;

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
		double m_startTime = 0.0;
		float m_physicsAccumulator = 0.0f;
	};
}

#endif
