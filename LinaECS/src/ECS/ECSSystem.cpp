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

#include "ECS/ECSSystem.hpp"  
#include "Utility/Log.hpp"
#include "ECS/Components/TransformComponent.hpp"
namespace LinaEngine::ECS
{
	bool ECSSystemList::RemoveSystem(BaseECSSystem& system)
	{
		for (unsigned int i = 0; i < m_systems.size(); i++)
		{
			// If the addr of the target system matches any system, erase it from the array.
			if (&system == m_systems[i])
			{
				m_systems.erase(m_systems.begin() + i);
				return true;
			}
		}

		return false;
	}

	entt::entity ECSRegistry::CreateEntity(const std::string& name)
	{
		entt::entity ent = create();
		emplace<ECSEntityData>(ent, ECSEntityData {false, false, name} );
		emplace<TransformComponent>(ent, TransformComponent());
		return ent;
	}

	ECSEntity ECSRegistry::GetEntity(const std::string& name)
	{
		auto singleView = view<ECSEntityData>();

		for(auto entity : singleView)
		{
			if (singleView.get<ECSEntityData>(entity).m_name.compare(name) == 0)
				return entity;
		}

		LINA_CORE_WARN("Entity with the name {0} could not be found, returning null entity.", name);
		return entt::null;
	}

}

