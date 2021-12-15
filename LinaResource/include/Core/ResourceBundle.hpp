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
Class: ResourceBundle

Represents a bundle, containing various packages for resource categories. Raw packages contain RAW data loaded from .linapackage files.

Timestamp: 12/22/2020 12:26:55 AM
*/

#pragma once

#ifndef ResourceBundle_HPP
#define ResourceBundle_HPP

// Headers here.
#include "ResourcesCommon.hpp"
#include "ResourcePackages.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace Lina
{
	namespace Event
	{
		class EventSystem;
	}
}

namespace Lina::Resources
{
	class ResourceBundle
	{

	private:

		friend class ResourceManager;
		friend class Packager;

		// Clears raw packages off from memory.
		void UnloadRawPackages();

		// Clears processed packages off from memory.
		void UnloadProcessedPackages();

		// Converts each raw package into a processed package & fills in the table.
		void ProcessRawPackages(Event::EventSystem* eventSys);

		// Pushes the given memory buffer to a resource map.
		void PushResourceFromMemory(const std::string& path, ResourceType type, std::vector<unsigned char>& data);

		// Loads all memory buffers stored in maps.
		void LoadAllMemoryMaps();

		// Calls LoadResourceFromFile on all resources within the folder.
		void LoadResourcesInFolder(Utility::Folder& root, ResourceProgressData* progData, const std::vector<ResourceType>& excludes, ResourceType onlyLoad = ResourceType::Unknown);

		// Loads given resource from a file path into memory.
		void LoadResourceFromFile(Utility::File& file, ResourceType type, ResourceProgressData* progData);

		std::unordered_map<ResourceType, RawPackage> m_rawPackages =
		{
			{ResourceType::Image, RawPackage()},
			{ResourceType::Material, RawPackage()},
			{ResourceType::Model, RawPackage()},
			{ResourceType::Audio, RawPackage()},
			{ResourceType::SPIRV, RawPackage()}
		};

		ImagePackage m_imagePackage;
		MeshPackage m_meshPackage;
		AudioPackage m_audioPackage;
		MaterialPackage m_materialPackage;		
		ShaderPackage m_shaderPackage;		

		std::unordered_map<std::string, std::vector<unsigned char>> m_audioParameters;
		std::unordered_map<std::string, std::vector<unsigned char>> m_imageParameters;
		std::unordered_map<std::string, std::vector<unsigned char>> m_modelParameters;
		std::unordered_map<std::string, std::vector<unsigned char>> m_shaderParameters;
		std::unordered_map<std::string, std::vector<unsigned char>> m_audios;
		std::unordered_map<std::string, std::vector<unsigned char>> m_images;
		std::unordered_map<std::string, std::vector<unsigned char>> m_hdrs;
		std::unordered_map<std::string, std::vector<unsigned char>> m_models;
		std::unordered_map<std::string, std::vector<unsigned char>> m_materials;
		std::unordered_map<std::string, std::vector<unsigned char>> m_shaders;
		std::unordered_map<std::string, std::vector<unsigned char>> m_shaderIncludes;
	};
}

#endif
