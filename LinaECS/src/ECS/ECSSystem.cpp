/*
Author: Inan Evin - Thanks to the lectures & contributions of Benny Bobaganoosh, thebennybox.
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ECSSystem
Timestamp: 4/8/2019 11:27:15 PM

*/

#include "ECS/ECSSystem.hpp"  

namespace LinaEngine::ECS
{
	
	bool ECSSystemList::RemoveSystem(BaseECSSystem& system)
	{

		// Iterate through systems.
		for (unsigned int i = 0; i < systems.size(); i++)
		{
			// If the addr of the target system matches any system, erase it from the array.
			if (&system == systems[i])
			{
				systems.erase(systems.begin() + i);
				return true;
			}
		}

		return false;
	}
	entt::entity ECSRegistry::CreateEntity(const std::string& name)
	{
		entt::entity ent = create();
		m_EntityNameMap[ent] = name;
		return ent;
	}

	void ECSRegistry::SetEntityName(ECSEntity entity, const std::string& name)
	{
		m_EntityNameMap[entity] = name;
	}


	std::string& ECSRegistry::GetEntityName(ECSEntity e)
	{
		return m_EntityNameMap[e];
	}
}

