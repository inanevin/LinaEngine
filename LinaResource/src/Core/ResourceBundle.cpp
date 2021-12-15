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
#include "Resources/ImageResource.hpp"
#include "Resources/MeshResource.hpp"
#include "Resources/AudioResource.hpp"
#include "Resources/MaterialResource.hpp"
#include "Resources/ShaderResource.hpp"
#include "Log/Log.hpp"
#include "EventSystem/EventSystem.hpp"

namespace Lina::Resources
{
	void ResourceBundle::UnloadRawPackages()
	{
		for (auto& p : m_rawPackages)
			p.second.clear();

		m_rawPackages.clear();
			
		LINA_TRACE("[Resource Bundle] -> Raw bundle unloaded.");
	}

	void ResourceBundle::UnloadProcessedPackages()
	{
		// Clear in-active packages only. (those whose buffers are immediately send to hardware.)
		for (auto& r : m_imagePackage)
			delete r.second;

		for (auto& r : m_audioPackage)
			delete r.second;

		for (auto& r : m_meshPackage)
			delete r.second;

		m_imagePackage.clear();
		m_audioPackage.clear();
		m_meshPackage.clear();

		/* DO NOT delete material & shader resources, as they are considered active resources and need to stay in-memory.*/
	}

	void ResourceBundle::ProcessRawPackages(Event::EventSystem* eventSys)
	{
		for (auto& package : m_rawPackages)
		{
			RawPackage& rawPackage = package.second;

			// For every resource in this package, 
			for (auto& resource : rawPackage)
			{
				// InitializeVulkan the corresponding package class from memory.
				if (package.first == ResourceType::Image)
				{
					ImageResource* img = new ImageResource();
					if (img->LoadFromMemory(resource.first, &resource.second[0], resource.second.size(), eventSys))
						m_imagePackage[resource.first] = img;
					else
						delete img;
				}
				else if (package.first == ResourceType::Mesh)
				{
					//MeshResource* mesh = new MeshResource();
					//if(mesh->LoadFromMemory(resource.first, &resource.second[0], resource.second.size()))
					//	m_meshPackage[resource.first] = mesh;
					//else
					//	delete mesh;
				}
				else if (package.first == ResourceType::Audio)
				{
					AudioResource* aud = new AudioResource();
					if (aud->LoadFromMemory(resource.first, &resource.second[0], resource.second.size(), eventSys))
						m_audioPackage[resource.first] = aud;
					else 
						delete aud;
				}
				else if (package.first == ResourceType::Material)
				{
					MaterialResource* mat = new MaterialResource();
					if (mat->LoadFromMemory(resource.first, &resource.second[0], resource.second.size(), eventSys))
						m_materialPackage[resource.first] = mat;
					else
						delete mat;
				}
				else if (package.first == ResourceType::SPIRV)
				{
					ShaderResource* shader = new ShaderResource();
					if (shader->LoadFromMemory(resource.first, &resource.second[0], resource.second.size(), eventSys))
						m_shaderPackage[resource.first] = shader;
					else
						delete shader;
				}
			}
		}

		LINA_TRACE("[Resource Bundle] -> Raw bundle processed into packages.");
		UnloadRawPackages();
	}

	void ResourceBundle::LoadResourcesInFolder(Utility::Folder& root, ResourceProgressData* progData)
	{
		for (auto& folder : root.m_folders)
		{
			LoadResourcesInFolder(folder, progData);
		}

		// Initialize each file into memory where they will persist during the editor lifetime.
		for (auto& file : root.m_files)
		{
			ResourceType resType = GetResourceType(file.m_extension);
			LoadResourceIntoMemory(file, resType, progData);
		}
	}

	void ResourceBundle::LoadResourceIntoMemory(Utility::File& file, ResourceType type, ResourceProgressData* progData)
	{
		progData->m_currentResourceName = file.m_fullPath;
		// InitializeVulkan the corresponding package class from memory.
		if (type == ResourceType::Image)
		{
			std::string paramsPath = file.m_folderPath + file.m_pureName + ".samplerparams";
			Event::EventSystem::Get()->Trigger<Event::ELoadImageResourceFromFile>(Event::ELoadImageResourceFromFile{ file.m_fullPath, paramsPath, false});

		}
		else if (type == ResourceType::HDR)
		{
			std::string paramsPath = file.m_folderPath + file.m_pureName + ".samplerparams";
			Event::EventSystem::Get()->Trigger<Event::ELoadImageResourceFromFile>(Event::ELoadImageResourceFromFile{ file.m_fullPath, paramsPath, true });
		}
		else if (type == ResourceType::Mesh)
		{
			std::string paramsPath = file.m_folderPath + file.m_pureName + ".modelparams";
			Event::EventSystem::Get()->Trigger<Event::ELoadModelResourceFromFile>( Event::ELoadModelResourceFromFile{ file.m_fullPath, paramsPath} );
		}
		else if (type == ResourceType::Audio)
		{
			std::string paramsPath = file.m_folderPath + file.m_pureName + ".audioparams";
			Event::EventSystem::Get()->Trigger<Event::ELoadAudioResourceFromFile>(Event::ELoadAudioResourceFromFile{ file.m_fullPath, paramsPath });
		}
		else if (type == ResourceType::Material)
		{
			Event::EventSystem::Get()->Trigger<Event::ELoadMaterialResourceFromFile>(Event::ELoadMaterialResourceFromFile{ file.m_fullPath });
		}
		else if (type == ResourceType::GLSL)
		{
			Event::EventSystem::Get()->Trigger<Event::ELoadShaderResourceFromFile>(Event::ELoadShaderResourceFromFile{ file.m_fullPath });
		}
	}

}