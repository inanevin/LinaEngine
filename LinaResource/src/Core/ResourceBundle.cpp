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


#include "Core/ResourceBundle.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/ResourceManager.hpp"
#include "Math/Math.hpp"
#include "Log/Log.hpp"

namespace Lina::Resources
{
	MemoryEntry::~MemoryEntry()
	{
		m_data.clear();
		LINA_TRACE("Memory entry popped");
	}

	void ResourceBundle::PushResourceFromMemory(const std::string& path, ResourceType type, std::vector<unsigned char>& data)
	{
		StringIDType sid = StringID(path.c_str()).value();
		
		int priority = 100;
		std::string paramsExtension = "";
		
		if (type == ResourceType::AudioData || type == ResourceType::ModelAssetData || type == ResourceType::ImageData)
		{
			m_memoryResourcesParams.push_back(MemoryEntry(priority, type, path, data, paramsExtension));
		}
		else
		{
			if (type == ResourceType::GLH)
				priority = 0;
			else if (type == ResourceType::GLSL)
				priority = 1;
			else if (type == ResourceType::Audio)
				paramsExtension = ".linaaudiodata";
			else if (type == ResourceType::Model)
				paramsExtension = ".linamodeldata";
			else if (type == ResourceType::Image || type == ResourceType::HDR)
				paramsExtension = ".linaimagedata";
		
			m_memoryResources.push(MemoryEntry(priority, type, path, data, paramsExtension));
		}
		
		
		data.clear();
	}

	void ResourceBundle::LoadAllMemoryMaps()
	{
		while (!m_memoryResources.empty())
		{
			MemoryEntry entry = m_memoryResources.top();
		
			std::string nameOnly = Utility::GetFileWithoutExtension(entry.m_path);
			std::string paramsName = nameOnly + entry.m_paramsExtension;
		
			bool wasParamFound = false;
			for (auto& paramEntry : m_memoryResourcesParams)
			{
				
				if (paramEntry.m_path.compare(paramsName) == 0)
				{
					Event::EventSystem::Get()->Trigger<Event::ELoadResourceFromMemory>(Event::ELoadResourceFromMemory{ entry.m_type,
						entry.m_path, &entry.m_data[0], entry.m_data.size(), paramEntry.m_path, &paramEntry.m_data[0], paramEntry.m_data.size()
						});
					
					wasParamFound = true;
					break;
				}
			}
		
			if (!wasParamFound)
			{
				Event::EventSystem::Get()->Trigger<Event::ELoadResourceFromMemory>(Event::ELoadResourceFromMemory{ entry.m_type, entry.m_path, &entry.m_data[0], entry.m_data.size() , "", nullptr, 0});
			}
		
			Event::EventSystem::Get()->Trigger<Event::EResourceLoadCompleted>(Event::EResourceLoadCompleted{entry.m_type, StringID(entry.m_path.c_str()).value()});
			m_memoryResources.pop();
		}
		m_memoryResourcesParams.clear();
	}

	void ResourceBundle::LoadResourcesInFolder(Utility::Folder& root,const std::vector<ResourceType>& excludes, ResourceType onlyLoad)
	{
		for (auto& folder : root.m_folders)
		{
			LoadResourcesInFolder(folder,  excludes, onlyLoad);
		}

		// Initialize each file into memory where they will persist during the editor lifetime.
		for (auto& file : root.m_files)
		{
			ResourceType resType = GetResourceType(file.m_extension);

			if (onlyLoad != ResourceType::Unknown && resType != onlyLoad)
				continue;

			if (excludes.size() != 0)
			{
				for (const auto rt : excludes)
				{
					if (resType == rt)
						continue;
				}
			}

			ResourceManager::s_currentProgressData.m_currentResourceName = file.m_fullPath;
			ResourceManager::s_currentProgressData.m_currentProcessedFiles++;
			ResourceManager::s_currentProgressData.m_currentProgress = ((float)ResourceManager::s_currentProgressData.m_currentProcessedFiles / (float)ResourceManager::s_currentProgressData.m_currentTotalFiles) * 100.0f;
			ResourceManager::s_currentProgressData.m_currentProgress = Lina::Math::Clamp(ResourceManager::s_currentProgressData.m_currentProgress, 0.0f, 100.0f);
			ResourceManager::TriggerResourceUpdatedEvent();
			LoadResourceFromFile(file, resType);
		}
	}

	void ResourceBundle::LoadResourceFromFile(Utility::File& file, ResourceType type)
	{
		std::string paramsPath = "";

		if (type == ResourceType::Image)
			paramsPath = file.m_folderPath + file.m_pureName + ".linaimagedata";
		else if (type == ResourceType::HDR)
			paramsPath = file.m_folderPath + file.m_pureName + ".linaimagedata";
		else if (type == ResourceType::Model)
			paramsPath = file.m_folderPath + file.m_pureName + ".linamodeldata";
		else if (type == ResourceType::Audio)
			paramsPath = file.m_folderPath + file.m_pureName + ".linaaudiodata";

		Event::EventSystem::Get()->Trigger<Event::ELoadResourceFromFile>(Event::ELoadResourceFromFile{type, file.m_fullPath, paramsPath});
		Event::EventSystem::Get()->Trigger<Event::EResourceLoadCompleted>(Event::EResourceLoadCompleted{ type, StringID(file.m_fullPath.c_str()).value() });

	}

}