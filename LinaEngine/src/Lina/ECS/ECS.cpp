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
Timestamp: 4/8/2019 6:03:20 PM

*/

#include "LinaPch.hpp"
#include "ECS.hpp"  

namespace LinaEngine
{
	ECS::~ECS()
	{
		// Remove components.
		for (Map<uint32, Array<uint8>>::iterator it = components.begin(); it != components.end(); ++it)
		{
			size_t typeSize = BaseECSComponent::getTypeSize(it->first);
			ECSComponentFreeFunction freefn = BaseECSComponent::getTypeFreeFunction(it->first);

			// Iterate every single component in the memory block by size.
			for (uint32 i = 0; i < it->second.size(); i += typeSize)
			{
				freefn((BaseECSComponent*)&(it->second[i]));
			}
		}

		// Remove entities. TODO: Maybe better optimize memory management.
		for (uint32 i = 0; entities.size(); i++)
		{
			delete entities[i];
		}
	}

	EntityHandle ECS::MakeEntity(BaseECSComponent* entityComponents, const uint32* componentIDs, size_t numComponents)
	{
		Pair<uint32, Array<Pair<uint32, uint32>>>* newEntity = new Pair<uint32, Array<Pair<uint32, uint32>>>();
		EntityHandle handle = (EntityHandle)newEntity;

		// Iterate through components
		for (uint32 i = 0; i < numComponents; i++)
		{
			// Delete entity & return if type is invalid.
			if (!BaseECSComponent::isTypeValid(componentIDs[i]))
			{
				LINA_CORE_ERR("ECS: Component type is not valid! {0}", componentIDs[i]);
				delete newEntity;
				return NULL_ENTITY_HANDLE;
			}

			ECSComponentCreateFunction createfn = BaseECSComponent::getTypeCreateFunction(componentIDs[i]);
			Pair<uint32, uint32> newPair;
			newPair.first = componentIDs[i];
			newPair.second = createfn(components[componentIDs[i]], handle, &entityComponents[i]);
			newEntity->second.push_back(newPair);
		}

		newEntity->first = entities.size();
		entities.push_back(newEntity);
		return handle;
	}

	void ECS::RemoveEntity(EntityHandle handle)
	{
		Array<Pair<uint32, uint32>>& entity = HandleToEntity(handle);

		for (uint32 i = 0; i < entity.size(); i++)
		{
			RemoveComponentInternal(entity[i].first, entity[i].second);
		}

		// Where our entity in the list of entities.
		uint32 destIndex = HandleToEntityIndex(handle);
		uint32 srcIndex = entities.size() - 1;

		// Delete the entity.
		delete entities[destIndex];

		// Move the last entity in the list to where the entity being deleted is.
		entities[destIndex] = entities[srcIndex];
		entities.pop_back();
	}

	void ECS::RemoveComponentInternal(uint32 componentID, uint32 index)
	{

	}

}

