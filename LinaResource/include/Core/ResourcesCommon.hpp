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
Class: ResourcesCommon

Common free funcs & definitions for resources classes.

Timestamp: 12/19/2020 6:42:13 PM
*/

#pragma once

#ifndef ResourcesCommon_HPP
#define ResourcesCommon_HPP

// Headers here.
#include "Resources/ImageResource.hpp"
#include "Resources/MeshResource.hpp"
#include "Resources/AudioResource.hpp"
#include "Resources/MaterialResource.hpp"
#include "Resources/ShaderResource.hpp"
#include "Utility/StringId.hpp"
#include <string>

namespace Lina::Resources
{
#define RESOURCEPACKAGE_EXTENSION ".linapackage"
#define IMAGEPACKAGE_PATH "Resources/Packages/images.linapackage"
#define PACKAGE_PASS L"1234"

	typedef std::unordered_map<StringIDType, ImageResource*> ImagePackage;
	typedef std::unordered_map<StringIDType, MeshResource*> MeshPackage;
	typedef std::unordered_map<StringIDType, AudioResource*> AudioPackage;
	typedef std::unordered_map<StringIDType, MaterialResource*> MaterialPackage;
	typedef std::unordered_map<StringIDType, ShaderResource*> ShaderPackage;
	typedef std::unordered_map<StringIDType, std::vector<unsigned char>> RawPackage;

	enum class ResourceProgressState
	{
		None,
		Pending,
		InProgress,
		Succeeded,
		Failed
	};

	enum class ResourceType
	{
		Unknown,
		Mesh,
		Image,
		Audio,
		Material,
		Shader
	};

	extern std::unordered_map<StringIDType, ResourceType> m_resourceTypeTable;

	class ResourceProgressData
	{
	public:
		
		ResourceProgressData() {};
		~ResourceProgressData() {  };

		ResourceProgressState m_state = ResourceProgressState::None;
		std::string m_currentResourceName = "";
		std::string m_progressTitle = "";
		int m_currentProgress = 0;

	private:
		ResourceProgressData(const ResourceProgressData&); // = delete
		ResourceProgressData& operator=(const ResourceProgressData&); // = delete

	};

	extern ResourceType GetResourceType(const std::string& extension);
}

#endif
