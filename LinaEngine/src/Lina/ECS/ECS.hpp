/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ECS
Timestamp: 4/8/2019 5:43:51 PM

*/

#pragma once

#ifndef ECS_HPP
#define ECS_HPP

#include "ECSSystem.hpp"

namespace LinaEngine
{
	class ECS
	{
	public:

		ECS() {}
		~ECS();
		
		/* Entity Creator */
		EntityHandle MakeEntity(BaseECSComponent* components, const uint32* componentIDs, size_t numComponents);
		
		/* Removes an entity from the sys */
		void RemoveEntity(EntityHandle handle);

		/* Adds component to an entity */
		template<class Component>
		FORCEINLINE void AddComponent(EntityHandle entity, Component* component)
		{
			AddComponentInternal(HandleToEntity(entity), Component::ID, component);
		}

		/* Removes component from an entity */
		template<class Component>
		void RemoveComponent(EntityHandle entity);

		/* Gets the typed component out of the entity. */
		template<class Component>
		void GetComponent(EntityHandle entity);

		/* Adds a system */
		FORCEINLINE void AddSystem(BaseECSSystem& system)
		{
			systems.push_back(&system);
		}

		/* System Tick */
		void UpdateSystems(float delta);
		
		/* Remove system */
		void RemoveSystem(BaseECSSystem& system);

	private:

		/* Array of ECS systems */
		Array<BaseECSSystem*> systems;

		/* Map of id & for each id a seperate array for each comp type */
		Map<uint32, Array<uint8>> components;
		
		/* Entities with pair ids. */
		Array<Pair<uint32, Array<Pair<uint32, uint32>>>*> entities;

		/* Converts an entity handle to raw type */
		FORCEINLINE Pair<uint32, Array<Pair<uint32, uint32> > >* HandleToRawType(EntityHandle handle)
		{
			return (Pair<uint32, Array<Pair<uint32, uint32> > >*)handle;
		}

		/* Pulls the index out of an entity handle. */
		FORCEINLINE uint32 HandleToEntityIndex(EntityHandle handle)
		{
			return HandleToRawType(handle)->first;
		}

		/* Converts an entity handle to entity type */
		FORCEINLINE Array<Pair<uint32, uint32> >& HandleToEntity(EntityHandle handle)
		{
			return HandleToRawType(handle)->second;
		}

		void DeleteComponent(uint32 componentID, uint32 index);
		void AddComponentInternal(Array<Pair<uint32, uint32>>& entity, uint32 componentID, BaseECSComponent* component);

		NULL_COPY_AND_ASSIGN(ECS);
	};

}


#endif