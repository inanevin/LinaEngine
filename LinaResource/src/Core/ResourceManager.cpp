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
#include "Utility/UtilityFunctions.hpp"

namespace Lina::Resources
{
	ResourceManager* ResourceManager::s_resourceManager = nullptr;
	ResourceProgressData ResourceManager::s_currentProgressData;

	void ResourceManager::LoadEditorResources()
	{
		LINA_WARN("[Resource Manager] -> Loading Editor Resources");

		// Find resources.
		Utility::Folder root;
		root.m_fullPath = "resources/";
		Utility::ScanFolder(root, true, &s_currentProgressData.m_currentTotalFiles);

		// Set progress & fill.
		s_currentProgressData.m_state = ResourceProgressState::Pending;
		s_currentProgressData.m_progressTitle = "Loading resources...";
		s_currentProgressData.m_state = ResourceProgressState::InProgress;
		s_currentProgressData.m_currentProcessedFiles = 0;
		std::unordered_map<std::string, ResourceType> filledResources;

		// Load all editor resources, first only load the shader includes, then shaders, then the rest.
		std::vector<ResourceType> excludes;
		m_bundle.LoadResourcesInFolder(root, excludes, ResourceType::GLH);
		m_bundle.LoadResourcesInFolder(root, excludes, ResourceType::GLSL);

		excludes.push_back(ResourceType::GLH);
		excludes.push_back(ResourceType::GLSL);
		m_bundle.LoadResourcesInFolder(root, excludes, ResourceType::Unknown);

		Event::EventSystem::Get()->Trigger<Event::EAllResourcesLoaded>(Event::EAllResourcesLoaded{});
		ResetProgress();
	}

	void ResourceManager::Shutdown()
	{
		// Make sure we don't have any packing/unpacking going on.
		if (m_future.valid())
		{
			m_future.cancel();
			m_future.get();
		}

		LINA_TRACE("[Resource Manager] -> Shutdown");
	}

	void Lina::Resources::ResourceManager::ResetProgress()
	{
		s_currentProgressData.m_currentProcessedFiles = s_currentProgressData.m_currentTotalFiles = 0;
		s_currentProgressData.m_currentResourceName = s_currentProgressData.m_progressTitle = "";
		s_currentProgressData.m_state = ResourceProgressState::None;
		s_currentProgressData.m_currentProgress = 0.0f;
	}

	void Lina::Resources::ResourceManager::TriggerResourceUpdatedEvent()
	{
		Event::EventSystem::Get()->Trigger<Event::EResourceLoadUpdated>(Event::EResourceLoadUpdated{ ResourceManager::s_currentProgressData.m_currentResourceName, ResourceManager::s_currentProgressData.m_currentProgress });
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
		m_packager.PackageFileset(filesToPack, path + "/" + name + RESOURCEPACKAGE_EXTENSION, PACKAGE_PASS);
	}


	void ResourceManager::AddAllResourcesToPack(std::vector<std::string>& resources, Utility::Folder& folder)
	{
		for (auto& childFolder : folder.m_folders)
			AddAllResourcesToPack(resources, childFolder);

		for (auto& file : folder.m_files)
			resources.push_back(file.m_fullPath);
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
		s_currentProgressData.m_progressTitle = "Unpacking level resources...";
		s_currentProgressData.m_currentResourceName = fullBundlePath;

		// Start unpacking process, preceeded & followed by an event dispatch.
		// m_eventSys->Trigger<Event::EResourceProgressStarted>();

		// Start unpacking.
		std::unordered_map<std::string, ResourceType> unpackedResources;
		m_packager.Unpack(fullBundlePath, PACKAGE_PASS, &m_bundle, unpackedResources);

		m_bundle.LoadAllMemoryMaps();

		// Set progress end.
		ResetProgress();
	}

}
