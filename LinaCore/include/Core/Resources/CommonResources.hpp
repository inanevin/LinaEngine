/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#pragma once

#include "Common/StringID.hpp"
#include "Common/Data/String.hpp"
#include "Common/Data/HashSet.hpp"
#include "Common/Common.hpp"
#include "Common/Data/HashSet.hpp"

namespace Lina
{
	class Resource;

	typedef uint64 ResourceID;

#define RESOURCE_ID_ENGINE_SPACE	 UINT64_MAX - 2000
#define RESOURCE_ID_CUSTOM_SPACE	 UINT64_MAX - 10000
#define RESOURCE_ID_CUSTOM_SPACE_MAX RESOURCE_ID_ENGINE_SPACE
#define RESOURCE_ID_MAX				 RESOURCE_ID_CUSTOM_SPACE - 1

	enum class PackageType
	{
		Default,
		Package1,
		Package2,
		PackageLevels,
	};

	enum ResourceFlags
	{
	};

	struct ResourceDef
	{
		ResourceID id		  = 0;
		String	   name		  = "";
		TypeID	   tid		  = 0;
		OStream	   customMeta = {};

		bool operator==(const ResourceDef& other) const
		{
			return id == other.id;
		}
	};

	struct ResourceDefHash
	{
		std::size_t operator()(const ResourceDef& s) const noexcept
		{
			return std::hash<ResourceID>{}(s.id);
		}
	};

	struct ResourceLoadTask
	{
		Taskflow		  tf;
		Vector<Resource*> resources;
		Atomic<bool>	  isCompleted = false;
		int32			  id		  = 0;
		uint64			  startTime	  = 0;
		uint64			  endTime	  = 0;
	};

	enum class ResourceType
	{
		ExternalSource,
		EngineCreated,
	};

	typedef HashSet<ResourceDef, ResourceDefHash> ResourceDefinitionList;
	typedef HashSet<Resource*>					  ResourceList;

} // namespace Lina
