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


#include "Core/ResourceManager.hpp"  
#include "Log/Log.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/ResourcesCommon.hpp"
#include "Resources/MaterialResource.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Resources
{
	static float t = 0.0f;
	static bool loaded = false;

	ResourceManager* ResourceManager::s_resourceManager = nullptr;

	void ResourceManager::DebugLevelLoad(Event::ETick& e)
	{
		t += e.m_deltaTime;
		if (!loaded && t > 3.0f)
		{
			loaded = true;
			// ExportLevel("", "default");
		}
	}

	void ResourceManager::OnStartGame(Event::EStartGame& e)
	{
		LINA_TRACE("[Resource Manager] -> Startup");


	}

	void ResourceManager::OnEndGame(Event::EEndGame& e)
	{
		// Make sure we don't have any packing/unpacking going on.
		if (m_future.valid())
		{
			m_future.cancel();
			m_future.get();
		}

		LINA_TRACE("[Resource Manager] -> Shutdown");
	}


	void ResourceManager::AddAllResourcesToPack(std::vector<std::string>& resources, Utility::Folder& folder)
	{
		for (auto& childFolder : folder.m_folders)
			AddAllResourcesToPack(resources, childFolder);

		for (auto& file : folder.m_files)
			resources.push_back(file.m_fullPath);
	}

	void ResourceManager::LoadEditorResources()
	{
		LINA_WARN("[Resource Manager] -> Loading Editor Resources");

		m_currentProgressData.m_state = ResourceProgressState::Pending;

		// Find resources.
		Utility::Folder root;
		root.m_fullPath = "resources/";
		Utility::ScanFolder(root, true);

		// Set progress & fill.
		m_currentProgressData.m_progressTitle = "Loading resources...";
		m_currentProgressData.m_state = ResourceProgressState::InProgress;
		m_currentProgressData.m_currentProgress = 0.0f;
		std::unordered_map<std::string, ResourceType> filledResources;

		// Load all editor resources, first only load shaders, then load the rest..
		m_bundle.LoadResourcesInFolder(root, &m_currentProgressData, ResourceType::GLSL);
		m_bundle.LoadResourcesInFolder(root, &m_currentProgressData, ResourceType::Unknown, ResourceType::GLSL);

		// Notify listeners that unpacking has finished.
		m_currentProgressData.m_state = ResourceProgressState::None;
	}


	void ResourceManager::ImportLevel(const std::string& path, const std::string& levelName, LevelData& levelData)
	{
		
		if (!Utility::FileExists(path + "/" + levelName + ".linalevel"))
		{
			LINA_ERR("Level you are trying to load does not exists. {0}, {1}", path, levelName);
			return;
		}

		LevelResource::ImportLevel(path, levelName, levelData);

		
		return;
		// TODO: Unload current level.

		// Make sure we set state before multithreading.
		std::string fullLevelPath = path + levelName + ".linalevel";
		m_currentProgressData.m_state = ResourceProgressState::InProgress;
		m_currentProgressData.m_progressTitle = "Importing level...";
		m_currentProgressData.m_currentResourceName = fullLevelPath;

		// Make sure we don't have any packing/unpacking going on.
		m_taskflow.clear();
		m_executor.wait_for_all();

		// LoadFromFile the level.
		m_activeLevel.LoadFromFile(fullLevelPath, *m_ecs);

		// If not in editor, load & unpack the bundle associated with this level, on a seperate thread.
		if (m_appMode == ApplicationMode::Standalone)
		{
			std::string fullBundlePath = path + levelName + ".linabundle";
			m_currentProgressData.m_progressTitle = "Unpacking level resources...";
			m_currentProgressData.m_currentResourceName = fullBundlePath;

			m_taskflow.emplace([=]()
				{
					// Start unpacking process, preceeded & followed by an event dispatch.
					m_eventSys->Trigger<Event::EResourceProgressStarted>();

					// Start unpacking.
					std::unordered_map<std::string, ResourceType> unpackedResources;
					m_packager.Unpack(fullBundlePath, PACKAGE_PASS, &m_bundle, &m_currentProgressData, unpackedResources);

					// Create processed resource instances from raw data.
					m_bundle.ProcessRawPackages(m_eventSys);

					// Unload all processed packages since the listeners of AddResourceReference already uploaded the buffers
					// From the packages to necessary memory blocks.
					m_bundle.UnloadProcessedPackages();

					// Set progress end.
					m_currentProgressData.m_state = ResourceProgressState::None;
					m_currentProgressData.m_progressTitle = "";
					m_currentProgressData.m_currentResourceName = "";
					m_currentProgressData.m_currentProgress = 0;

					// Notify listeners that unpacking has finished.
					m_eventSys->Trigger<Event::EResourceProgressEnded>();

					// Notify listeners of level load.
					// m_eventSys->Trigger<Event::ELevelLoaded>();
				});

			m_future = m_executor.run(m_taskflow);
		}
		else
		{
			// m_eventSys->Trigger<Event::ELevelLoaded>();

		}

	}

	void ResourceManager::ExportLevel(const std::string& path, const std::string& levelName, LevelData& levelData)
	{
		// Setup progress data.
		m_currentProgressData.m_state = ResourceProgressState::InProgress;
		m_currentProgressData.m_progressTitle = "Exporting level...";
		m_currentProgressData.m_currentResourceName = levelName + ".linalevel";
		LevelResource::ExportLevel(path, levelName, levelData);
	}

	void ResourceManager::PackageProject(const std::string& path, const std::string& name)
	{
		// Find out which resources to export.
		std::vector<std::string> filesToPack;

		Utility::Folder root;
		root.m_fullPath = "resources/";
		Utility::ScanFolder(root);
		AddAllResourcesToPack(filesToPack, root);
		
		// Export resources.
		m_packager.PackageFileset(filesToPack, path + "/" + name + RESOURCEPACKAGE_EXTENSION, PACKAGE_PASS, &m_currentProgressData);
	}

	void ResourceManager::ImportResourceBundle(const std::string& path, const std::string& name)
	{
		const std::string fullPath = path + name + RESOURCEPACKAGE_EXTENSION;
		if (!Utility::FileExists(fullPath))
		{
			LINA_ERR("Package does not exist, aborting import. {0}", fullPath);
			return;
		}
		std::string fullBundlePath = fullPath;
		m_currentProgressData.m_progressTitle = "Unpacking level resources...";
		m_currentProgressData.m_currentResourceName = fullBundlePath;

		// Start unpacking process, preceeded & followed by an event dispatch.
		// m_eventSys->Trigger<Event::EResourceProgressStarted>();

		// Start unpacking.
		std::unordered_map<std::string, ResourceType> unpackedResources;
		m_packager.Unpack(fullBundlePath, PACKAGE_PASS, &m_bundle, &m_currentProgressData, unpackedResources);

		m_bundle.LoadAllMemoryMaps();

		// Set progress end.
		m_currentProgressData.m_state = ResourceProgressState::None;
		m_currentProgressData.m_progressTitle = "";
		m_currentProgressData.m_currentResourceName = "";
		m_currentProgressData.m_currentProgress = 0;
	}

}
