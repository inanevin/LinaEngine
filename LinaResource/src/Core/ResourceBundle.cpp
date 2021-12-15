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
				else if (package.first == ResourceType::Model)
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

	void ResourceBundle::PushResourceFromMemory(const std::string& path, ResourceType type, std::vector<unsigned char>& data)
	{
		StringIDType sid = StringID(path.c_str()).value();

		if (type == ResourceType::Image)
			m_images[path] = data;
		else if (type == ResourceType::ImageParams)
			m_imageParameters[path] = data;
		else if (type == ResourceType::HDR)
			m_imageParameters[path] = data;
		else if (type == ResourceType::Model)
			m_models[path] = data;
		else if (type == ResourceType::ModelParams)
			m_modelParameters[path] = data;
		else if (type == ResourceType::Audio)
			m_audios[path] = data;
		else if (type == ResourceType::AudioParams)
			m_audioParameters[path] = data;
		else if (type == ResourceType::Material)
			m_materials[path] = data;
		else if (type == ResourceType::GLSL)
			m_shaders[path] = data;
		else if (type == ResourceType::GLH)
			m_shaderIncludes[path] = data;

		data.clear();
	}

	void ResourceBundle::LoadAllMemoryMaps()
	{
		
		for (auto& shaderInc : m_shaderIncludes)
		{

		}
		for (auto& shader : m_shaders)
			Event::EventSystem::Get()->Trigger<Event::ELoadShaderResourceFromMemory>(Event::ELoadShaderResourceFromMemory{ shader.first, &shader.second[0], shader.second.size() });

		for (auto& audio : m_audios)
		{
			std::string nameOnly = Utility::GetFileWithoutExtension(audio.first);
			std::string paramsName = nameOnly + ".audioparams";

			// If parameters were pushed into the map, load with params, if not, load as default.
			if (m_audioParameters.find(paramsName) != m_audioParameters.end())
			{
				Event::EventSystem::Get()->Trigger<Event::ELoadAudioResourceFromMemory>(Event::ELoadAudioResourceFromMemory{
					audio.first, &audio.second[0], audio.second.size(),
					paramsName, &m_audioParameters[paramsName][0], m_audioParameters[paramsName].size() });
			}
			else
			{
				Event::EventSystem::Get()->Trigger<Event::ELoadAudioResourceFromMemory>(Event::ELoadAudioResourceFromMemory{
					audio.first, &audio.second[0], audio.second.size(),
					"", nullptr, 0 });
			}
		}

		for (auto& image : m_images)
		{
			std::string nameOnly = Utility::GetFileWithoutExtension(image.first);
			std::string paramsName = nameOnly + ".samplerparams";

			// If parameters were pushed into the map, load with params, if not, load as default.
			if (m_imageParameters.find(paramsName) != m_imageParameters.end())
			{
				Event::EventSystem::Get()->Trigger<Event::ELoadImageResourceFromMemory>(Event::ELoadImageResourceFromMemory{
					image.first, &image.second[0], image.second.size(),
					paramsName, &m_imageParameters[paramsName][0], m_imageParameters[paramsName].size() , false });
			}
			else
			{
				Event::EventSystem::Get()->Trigger<Event::ELoadImageResourceFromMemory>(Event::ELoadImageResourceFromMemory{
					image.first, &image.second[0], image.second.size(),
					"", nullptr, 0, false });
			}
		}

		for (auto& model : m_models)
		{
			std::string nameOnly = Utility::GetFileWithoutExtension(model.first);
			std::string paramsName = nameOnly + ".modelparams";

			// If parameters were pushed into the map, load with params, if not, load as default.
			if (m_modelParameters.find(paramsName) != m_modelParameters.end())
			{
				Event::EventSystem::Get()->Trigger<Event::ELoadModelResourceFromMemory>(Event::ELoadModelResourceFromMemory{
					model.first, &model.second[0], model.second.size(),
					paramsName, &m_modelParameters[paramsName][0], m_modelParameters[paramsName].size() });
			}
			else
			{
				Event::EventSystem::Get()->Trigger<Event::ELoadModelResourceFromMemory>(Event::ELoadModelResourceFromMemory{
					model.first, &model.second[0], model.second.size(),
					"", nullptr, 0 });
			}
		}

		for (auto& material : m_materials)
			Event::EventSystem::Get()->Trigger<Event::ELoadMaterialResourceFromMemory>(Event::ELoadMaterialResourceFromMemory{ material.first, &material.second[0], material.second.size() });

		for (auto v : m_images)
			v.second.clear();

		for (auto v : m_imageParameters)
			v.second.clear();

		for (auto v : m_audioParameters)
			v.second.clear();

		for (auto v : m_audios)
			v.second.clear();

		for (auto v : m_modelParameters)
			v.second.clear();

		for (auto v : m_models)
			v.second.clear();

		for (auto v : m_materials)
			v.second.clear();

		for (auto v : m_shaders)
			v.second.clear();

		for (auto v : m_shaderIncludes)
			v.second.clear();

		m_shaderIncludes.clear();
		m_shaders.clear();
		m_materials.clear();
		m_audioParameters.clear();
		m_audios.clear();
		m_imageParameters.clear();
		m_images.clear();
		m_models.clear();
		m_modelParameters.clear();
	}

	void ResourceBundle::LoadResourcesInFolder(Utility::Folder& root, ResourceProgressData* progData, const std::vector<ResourceType>& excludes, ResourceType onlyLoad)
	{
		for (auto& folder : root.m_folders)
		{
			LoadResourcesInFolder(folder, progData, excludes, onlyLoad);
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

			LoadResourceFromFile(file, resType, progData);
			progData->m_currentResourceName = file.m_fullPath;
			progData->m_progressTitle = "Loading resources in folder...";
		}
	}

	void ResourceBundle::LoadResourceFromFile(Utility::File& file, ResourceType type, ResourceProgressData* progData)
	{
		if (type == ResourceType::Image)
		{
			std::string paramsPath = file.m_folderPath + file.m_pureName + ".samplerparams";
			Event::EventSystem::Get()->Trigger<Event::ELoadImageResourceFromFile>(Event::ELoadImageResourceFromFile{ file.m_fullPath, paramsPath, false });
		}
		else if (type == ResourceType::HDR)
		{
			std::string paramsPath = file.m_folderPath + file.m_pureName + ".samplerparams";
			Event::EventSystem::Get()->Trigger<Event::ELoadImageResourceFromFile>(Event::ELoadImageResourceFromFile{ file.m_fullPath, paramsPath, true });
		}
		else if (type == ResourceType::Model)
		{
			std::string paramsPath = file.m_folderPath + file.m_pureName + ".modelparams";
			Event::EventSystem::Get()->Trigger<Event::ELoadModelResourceFromFile>(Event::ELoadModelResourceFromFile{ file.m_fullPath, paramsPath });
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
		else if (type == ResourceType::GLH)
		{
			Event::EventSystem::Get()->Trigger<Event::ELoadShaderIncludeResourceFromFile>(Event::ELoadShaderIncludeResourceFromFile{ file.m_fullPath });
		}
		else if (type == ResourceType::GLSL)
		{
			Event::EventSystem::Get()->Trigger<Event::ELoadShaderResourceFromFile>(Event::ELoadShaderResourceFromFile{ file.m_fullPath });
		}
		else if (type == ResourceType::GLH)
		{
			Event::EventSystem::Get()->Trigger<Event::ELoadShaderIncludeResourceFromFile>(Event::ELoadShaderIncludeResourceFromFile{ file.m_fullPath });
		}
	}

}