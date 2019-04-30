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
}

