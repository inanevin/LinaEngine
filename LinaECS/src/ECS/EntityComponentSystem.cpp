/*
Author: Inan Evin - Thanks to the lectures & contributions of Benny Bobaganoosh, thebennybox.
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: EntityComponentSystem
Timestamp: 4/8/2019 6:03:20 PM

*/

#include "ECS/EntityComponentSystem.hpp"  
#include "PackageManager/PAMMemory.hpp"
#include "PackageManager/PAMMath.hpp"
#include "Utility/Log.hpp"
namespace LinaEngine::ECS
{
	EntityComponentSystem::~EntityComponentSystem()
	{
		// Remove components.
		for (std::map<uint32, LinaArray<uint8>>::iterator it = components.begin(); it != components.end(); ++it)
		{
			size_t typeSize = BaseECSComponent::GetTypeSize(it->first);
			ECSComponentFreeFunction freefn = BaseECSComponent::GetTypeFreeFunction(it->first);

			// Iterate every single component in the memory block by size.
			for (uint32 i = 0; i < it->second.size(); i += typeSize)
			{
				freefn((BaseECSComponent*)&it->second[i]);
			}
		}

		// Remove entities. TODO: Maybe better optimize memory management.
		for (uint32 i = 0; i < entities.size(); i++)
		{
			delete entities[i];
		}
	}

	EntityHandle EntityComponentSystem::MakeEntity(BaseECSComponent** entityComponents, const uint32* componentIDs, size_t numComponents)
	{
		// Create entity & handle
		std::pair<uint32, LinaArray<std::pair<uint32, uint32>>>* newEntity = new std::pair<uint32, LinaArray<std::pair<uint32, uint32>>>();
		EntityHandle handle = (EntityHandle)newEntity;

		// Iterate through components
		for (uint32 i = 0; i < numComponents; i++)
		{
			// Delete entity & return if type is invalid.
			if (!BaseECSComponent::isTypeValid(componentIDs[i]))
			{
				LINA_CORE_ERR("ECS: Component type is not valid! Component ID: {0} ", componentIDs[i]);
				delete newEntity;
				return NULL_ENTITY_HANDLE;
			}

			// Add components to the newly created entity.
			AddComponentInternal(handle, newEntity->second, componentIDs[i], entityComponents[i]);
		}

		// Set the index & push into the array.
		newEntity->first = entities.size();
		entities.push_back(newEntity);

		// Call OnMakeEntity on the listeners if valid.
		for (uint32 i = 0; i < listeners.size(); i++)
		{
			bool isValid = true;

			if (listeners[i]->ShouldNotifyAllEntityActions())
				listeners[i]->OnMakeEntity(handle);
			else
			{
				// Check if we find the component to be removed, if so, call method.
				for (uint32 j = 0; j < listeners[i]->GetComponentIDs().size(); j++)
				{
					bool hasComponent = false;
					// Find matching components & call callback on it.
					for (uint32 k = 0; k < numComponents; k++)
					{
						if (listeners[i]->GetComponentIDs()[j] == componentIDs[k])
						{
							hasComponent = true;
							break;
						}
					}

					if (!hasComponent)
					{
						isValid = false;
						break;
					}
				}

				if (isValid)
					listeners[i]->OnMakeEntity(handle);
			}
		}

		return handle;
	}

	void EntityComponentSystem::RemoveEntity(EntityHandle handle)
	{
		// Get entity from the handle.
		LinaArray<std::pair<uint32, uint32>>& entity = HandleToEntity(handle);

		// Call OnRemoveEntity callback on listeners if valid.
		for (uint32 i = 0; i < listeners.size(); i++)
		{
			// Get the related component IDs from the listeners.
			const LinaArray<uint32>& componentIDs = listeners[i]->GetComponentIDs();
			bool isValid = true;

			if (listeners[i]->ShouldNotifyAllEntityActions())
				listeners[i]->OnRemoveEntity(handle);
			else
			{
				// Check if we find the component to be removed, if so, call method.
				for (uint32 j = 0; j < componentIDs.size(); j++)
				{
					bool hasComponent = false;
					// Find matching components & call callback on it.
					for (uint32 k = 0; k < entity.size(); k++)
					{
						if (componentIDs[j] == entity[k].first)
						{
							hasComponent = true;
							break;
						}
					}

					if (!hasComponent)
					{
						isValid = false;
						break;
					}
				}

				if (isValid)
					listeners[i]->OnRemoveEntity(handle);
			}
		
		}

		

		// Delete the components first.
		for (uint32 i = 0; i < entity.size(); i++)
		{
			DeleteComponent(entity[i].first, entity[i].second);
		}

		// Where our entity in the list of entities.
		uint32 destIndex = HandleToEntityIndex(handle);
		uint32 srcIndex = entities.size() - 1;

		// Delete the entity.
		delete entities[destIndex];

		// Move the last entity in the list to where the entity being deleted is.
		entities[destIndex] = entities[srcIndex];
		entities[destIndex]->first = destIndex;
		entities.pop_back();
	}


	void EntityComponentSystem::UpdateSystems(ECSSystemList& systems, float delta)
	{
		// Components.
		LinaArray<BaseECSComponent*> componentParam;
		LinaArray<LinaArray<uint8>*> componentArrays;
		// Iterate through the systems.
		for (uint32 i = 0; i < systems.Size(); i++)
		{
			// Get the component types of the current system, if multiple we are going to do a different
			// deletion logic.
			const LinaArray<uint32>& componentTypes = systems[i]->GetComponentTypes();

		
			if (componentTypes.size() == 1)
			{
				// Get the type size
				size_t typeSize = BaseECSComponent::GetTypeSize(componentTypes[0]);

				// Get the components in this particular type
				LinaArray<uint8>& arr = components[componentTypes[0]];

				/* Update the components of the same type */
				for (uint32 j = 0; j < arr.size(); j += typeSize)
				{
					BaseECSComponent* component = (BaseECSComponent*)&arr[j];
					systems[i]->UpdateComponents(delta, &component);
				}
			}
			else
			{
				// If multiple types exist, switch the update logic.
				UpdateSystemMultipleComponentsInternal(i, systems, delta, componentTypes, componentParam, componentArrays);
			}
		}
	}


	void EntityComponentSystem::AddComponentInternal(EntityHandle handle, LinaArray<std::pair<uint32, uint32>>& entity, uint32 componentID, BaseECSComponent * component)
	{
		// Get the create functor for the component
		ECSComponentCreateFunction createfn = BaseECSComponent::GetTypeCreateFunction(componentID);

		// Assign the id for the new component & create function addr.
		std::pair<uint32, uint32> newPair;
		newPair.first = componentID;
		newPair.second = createfn(components[componentID], handle, component);

		// Push the newly created component to the target entity.
		entity.push_back(newPair);
	}

	void EntityComponentSystem::DeleteComponent(uint32 componentID, uint32 index)
	{
		// Get the components & free function.
		LinaArray<uint8>& arr = components[componentID];
		ECSComponentFreeFunction freefn = BaseECSComponent::GetTypeFreeFunction(componentID);
		
		// Get the type size & find the source index based on the component array.
		size_t typeSize = BaseECSComponent::GetTypeSize(componentID);
		uint32 srcIndex = arr.size() - typeSize;

		// Find the source component & call the free func.
		BaseECSComponent* sourceComponent = (BaseECSComponent*)&arr[srcIndex];
		BaseECSComponent* destComponent = (BaseECSComponent*)&arr[index];
		freefn(destComponent);

		// If the element in question is final in the array.
		if (index == srcIndex)
		{
			arr.resize(srcIndex);
			return;
		}

		// Move mem.
		Memory::memcpy(destComponent, sourceComponent, typeSize);

		// Iterate through sources & shift.
		LinaArray<std::pair<uint32,uint32>>& sourceComponents = HandleToEntity(sourceComponent->entity);
		for (uint32 i = 0; i < sourceComponents.size(); i++)
		{
			if (componentID == sourceComponents[i].first && srcIndex == sourceComponents[i].second)
			{
				sourceComponents[i].second = index;
				break;
			}
		}

		arr.resize(srcIndex);
	}

	bool EntityComponentSystem::RemoveComponentInternal(EntityHandle handle, uint32 componentID)
	{
		// Iterate through the components of the entity.
		LinaArray<std::pair<uint32, uint32>>& entityComponents = HandleToEntity(handle);
		for (uint32 i = 0; i < entityComponents.size(); i++)
		{
			if (componentID == entityComponents[i].first)
			{
				// Delete comp & resize.
				DeleteComponent(entityComponents[i].first, entityComponents[i].second);
				uint32 srcIndex = entityComponents.size() - 1;
				uint32 destIndex = i;
				entityComponents[destIndex] = entityComponents[srcIndex];
				entityComponents.pop_back();
				return true;
			}
		}

		return false;
	}


	BaseECSComponent* EntityComponentSystem::GetComponentInternal(LinaArray<std::pair<uint32, uint32>>& entityComponents, LinaArray<uint8>& arr, uint32 componentID)
	{
		for (uint32 i = 0; i < entityComponents.size(); i++)
		{
			if (componentID == entityComponents[i].first)
			{
				return (BaseECSComponent*)&arr[entityComponents[i].second];
			}
		}

		return nullptr;
	}

	void EntityComponentSystem::UpdateSystemMultipleComponentsInternal(uint32 index, ECSSystemList& systems, float delta, const LinaArray<uint32>& componentTypes, LinaArray<BaseECSComponent*>& componentParam, LinaArray<LinaArray<uint8>*>& componentArrays)
	{

		const LinaArray<uint32>& componentFlags = systems[index]->GetComponentFlags();

		componentParam.resize(Math::Max(componentParam.size(), componentTypes.size()));
		componentArrays.resize(Math::Max(componentParam.size(), componentTypes.size()));

		for (uint32 i = 0; i < componentTypes.size(); i++)
		{
			componentArrays[i] = &components[componentTypes[i]];
		}

		// Get the least common component.
		uint32 minSizeIndex = FindLeastCommonComponent(componentTypes, componentFlags);

		// Start with the first component type
		size_t typeSize = BaseECSComponent::GetTypeSize(componentTypes[minSizeIndex]);
		LinaArray<uint8>& arr = *componentArrays[minSizeIndex];

		// For every component type
		for (uint32 i = 0; i < arr.size(); i += typeSize)
		{
			componentParam[minSizeIndex] = (BaseECSComponent*)&arr[i];

			/******************** TODO: CACHE OPTIMIZATION ***********************/
			// Find the entity attached to the component.
			LinaArray<std::pair<uint32, uint32>>& entityComponents = HandleToEntity(componentParam[minSizeIndex]->entity);
			/******************** TODO: CACHE OPTIMIZATION ***********************/

			bool isValid = true;

			// If the entity has all the remaining components we are interested in, update it, otherwise skip it.
			for (uint32 j = 0; j < componentTypes.size(); j++)
			{
				if (j == minSizeIndex) continue;

				componentParam[j] = GetComponentInternal(entityComponents, *componentArrays[j], componentTypes[j]);

				// If the entity is not a match (null and the component is not optional ) for the system break.
				if (componentParam[j] == nullptr && (componentFlags[j] & BaseECSSystem::FLAG_OPTIONAL) == 0)
				{
					isValid = false;
					break;
				}
			}

			if (isValid)
			{
				systems[index]->UpdateComponents(delta, &componentParam[0]);
			}
		
		}

	}

	uint32 EntityComponentSystem::FindLeastCommonComponent(const LinaArray<uint32>& componentTypes, const LinaArray<uint32>& componentFlags)
	{
		uint32 minSize = (uint32)-1;
		uint32 minIndex = (uint32)-1;
		
		for (uint32 i = 0; i < componentTypes.size(); i++)
		{
			if ((componentFlags[i] & BaseECSSystem::FLAG_OPTIONAL) != 0) continue;
		
			size_t typeSize = BaseECSComponent::GetTypeSize(componentTypes[i]);

			// Actual # of components.
			uint32 size = components[componentTypes[i]].size() / typeSize;

			if (size <= minSize)
			{
				minSize = size;
				minIndex = i;
			}
		}

		return minIndex;
	}



}

