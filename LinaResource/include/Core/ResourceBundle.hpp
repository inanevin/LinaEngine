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

		// Fills in the corresponding processed package w/ the resource.
		void FillProcessedPackage(const std::string& path, ResourceType type, ResourceProgressData* progData, Event::EventSystem* eventSys);

		std::unordered_map<ResourceType, RawPackage> m_rawPackages =
		{
			{ResourceType::Image, RawPackage()},
			{ResourceType::Material, RawPackage()},
			{ResourceType::Mesh, RawPackage()},
			{ResourceType::Audio, RawPackage()}
		};

		ImagePackage m_imagePackage;
		MeshPackage m_meshPackage;
		AudioPackage m_audioPackage;
		MaterialPackage m_materialPackage;		
		ShaderPackage m_shaderPackage;		
	};
}

#endif
