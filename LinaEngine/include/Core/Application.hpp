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
#include "Core/Engine.hpp"

namespace Lina
{

	class Application
	{
	public:

#define DELTA_TIME_HISTORY 11

		Application();
		virtual ~Application() {};

		bool InstallLevel(Lina::World::Level& level, bool loadFromFile = false, const std::string& path = "", const std::string& levelName = "");
		void InitializeLevel(Lina::World::Level& level);
		void SaveLevelData(const std::string& folderPath, const std::string& fileName);
		void LoadLevelData(const std::string& folderPath, const std::string& fileName);
		void RestartLevel();
		void UninstallLevel();
		bool GetActiveLevelExists() { return m_activeLevelExists; }

		World::Level* GetCurrentLevel() { return m_currentLevel; }

		static Application& Get() { return *s_application; }

	protected:

		virtual void Initialize(ApplicationInfo& appInfo);
		void Run();

	private:

		// Callbacks.
		void OnLog(Event::ELog dump);
		bool OnWindowClose(Event::EWindowClosed);
		void OnWindowResize(Event::EWindowResized);

	private:

		// Active engines running in the application.
		static Application* s_application;
		World::Level* m_currentLevel = nullptr;
		Engine m_engine;
		ApplicationInfo m_appInfo;

		bool m_activeLevelExists = false;


	};

};


#endif