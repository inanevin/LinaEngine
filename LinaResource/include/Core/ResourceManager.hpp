/* 
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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
Class: PhysicsEnginePhysX

Responsible for managing all asset resources, both offline & in runtime

Timestamp: 5/1/2019 2:35:28 AM
*/

#pragma once

#ifndef ResourceEngine_HPP
#define ResourceEngine_HPP

#include "ResourceBundle.hpp"
#include "ResourcePackages.hpp"
#include "Resources/LevelResource.hpp"
#include "Utility/StringId.hpp"
#include "JobSystem/JobSystem.hpp"
#include "ECS/ECS.hpp"
#include "Utility/Packager.hpp"
#include "EventSystem/Events.hpp"

namespace Lina
{
	class Engine;

	namespace Utility
	{
		struct Folder;
	}

	namespace Event
	{
		class EventSystem;
	}
}

namespace Lina::Resources
{

	class ResourceManager
	{


	public:

		static ResourceManager* Get() { return s_resourceManager; }

		void ImportLevel(const std::string& path, const std::string& levelName, LevelData& levelData);
		void ExportLevel(const std::string& path, const std::string& levelName, LevelData& levelData);
		void PackageProject(const std::string& path, const std::string& name);
		void ImportResourceBundle(const std::string& path, const std::string& name);

		// Queries current progress data, useful for showing progress information.
		ResourceProgressData& GetCurrentProgressData() { return m_currentProgressData; }

	
	private:

		friend class Engine;
		ResourceManager() {};
		~ResourceManager() {};

		void AddAllResourcesToPack(std::vector<std::string>& resources, Utility::Folder& folder);
		void LoadEditorResources();
		void DebugLevelLoad(Event::ETick& e);
		void OnStartGame(Event::EStartGame& e);
		void OnEndGame(Event::EEndGame& e);

	private:

		static ResourceManager* s_resourceManager ;
		Event::EventSystem* m_eventSys = nullptr;
		ECS::Registry* m_ecs = nullptr;
		ResourceProgressData m_currentProgressData;
		Packager m_packager;
		Executor m_executor;
		TaskFlow m_taskflow;
		TaskFlow m_taskflowLoop;
		Future<void> m_future;
		Future<void> m_futureLoop;

	private:

		LevelResource m_activeLevel;
		ResourceBundle m_bundle;
		ApplicationMode m_appMode = ApplicationMode::Editor;

	};
}


#endif