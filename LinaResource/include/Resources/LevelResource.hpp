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
Class: LevelResource



Timestamp: 12/19/2020 3:54:38 PM
*/

#pragma once

#ifndef LevelResource_HPP
#define LevelResource_HPP

#include "Utility/StringId.hpp"
#include "Core/ResourcesCommon.hpp"
#include "ECS/ECS.hpp"
#include <cereal/cereal.hpp>
#include <set>
#include <vector>
#include <list>

namespace Lina::Resources
{
	class ResourceBundle;

	class LevelResource
	{

	public:

		

	private:

		friend class ResourceManager;
		friend class cereal::access;

		LevelResource() {};
		virtual ~LevelResource() {};

		inline void AddUsedResource(const std::string& path) { m_usedResources.push_back(path); }
		inline void RemoveUsedResource(const std::string& path) { m_usedResources.remove(path); }

		bool Export(const std::string& path, ECS::Registry& ecs);
		bool LoadFromFile(const std::string& path, ECS::Registry& ecs);

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_dummy);
		}

	private:

		int m_dummy;
		std::list<std::string> m_usedResources;
	};
}


#endif
