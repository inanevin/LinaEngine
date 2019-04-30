/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: InteractionWorld
Timestamp: 4/30/2019 7:43:50 PM

*/

#include "LinaPch.hpp"
#include "Core/InteractionWorld.hpp"  

namespace LinaEngine
{

	void InteractionWorld::OnMakeEntity(EntityHandle handle)
	{
		entities.push_back(handle);
	}

	void InteractionWorld::OnRemoveEntity(EntityHandle handle)
	{
		entityDump.push_back(handle);
	}

	void InteractionWorld::OnAddComponent(EntityHandle handle, uint32 id)
	{
		if (id == TransformComponent::ID)
		{
			if (ecs.GetComponent<ColliderComponent>(handle) != nullptr)
			{
				entities.push_back(handle);
			}
		}

		if (id == ColliderComponent::ID)
		{
			if (ecs.GetComponent<TransformComponent>(handle) != nullptr)
			{
				entities.push_back(handle);
			}
		}
	}

	void InteractionWorld::OnRemoveComponent(EntityHandle handle, uint32 id)
	{
		if (id == TransformComponent::ID || id == ColliderComponent::ID)
			entityDump.push_back(handle);
	}

	void InteractionWorld::Tick(float delta)
	{
		// Clear entity dump
		ClearEntityDump();

		// Find highest variance axis for AABBs
		int axis = FindHighestVarianceAxis();

		
	}

	void InteractionWorld::ClearEntityDump()
	{

		if (entityDump.size() == 0) return;

		// Check each entity if they have been placed into the dump
		for (size_t i = 0; i < entities.size(); i++)
		{
			bool isRemoved = false;
			do
			{
				isRemoved = false;
				for (size_t j = 0; j < entityDump.size(); j++)
				{
					if (entities[i] == entityDump[j])
					{
						// put the entity at the end of the list & pop it.
						entities.swap_remove(i);
						entityDump.swap_remove(j);
						isRemoved = true;
						break;
					}
				}

				if (isRemoved && entityDump.size() == 0) return;
			} while (isRemoved);
		}

		entityDump.clear();
	}

	int InteractionWorld::FindHighestVarianceAxis()
	{
		return 0;
	}
}

