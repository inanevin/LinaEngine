/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/24/2018 1:33:17 AM

*/

#include "pch.h"
#include "ECS/Lina_ECS.hpp"  

Lina_ECS::Lina_ECS()
{

}

Lina_ECS::~Lina_ECS()
{
	// Delete components for every single array.
	for (std::map<uint32, Lina_DSArray<uint8>>::iterator it = components.begin(); it != components.end(); ++it)
	{
		// Get type size and free functions of the iterated component.
		size_t typeSize = Lina_ECSBaseComponent::GetTypeSize(it->first);
		Lina_ECSComponentFreeFunction freefn = Lina_ECSBaseComponent::GetTypeFreeFunction(it->first);

		// Iterate the block of memory reserved for components.
		for (uint32 i = 0; i < it->second.size(); i += typeSize)
		{
			// Free every single component in the array.
			freefn((Lina_ECSBaseComponent*)&it->second[i]);
		}
	}

	// Delete entities.
	for (uint32 i = 0; i < entities.size(); i++)
		delete entities[i];
}

Lina_EntityHandle Lina_ECS::MakeEntity(Lina_ECSBaseComponent * entityComponents, const uint32 * componentIDs, size_t componentCount)
{
	// Create entity get handle.
	IndexEntityPair* newEntity = new Entity();
	Lina_EntityHandle handle = (Lina_EntityHandle)newEntity;

	// Handle components.
	for (uint32 i = 0; i < componentCount; i++)
	{
		// Check component id.
		if (!Lina_ECSBaseComponent::DoesTypeExist(componentIDs[i]))
		{
			DEBUG_LOG("ECS", LOG_ERROR, "'%u' Component type doesn't exist!", componentIDs[i]);
			delete newEntity;
			return NULL_ENTITY_HANDLE;
		}

		// Add component.
		AddComponentInternal(newEntity->second, componentIDs[i], &entityComponents[i]);
	}

	// assign index add entity to the list.
	newEntity->first = entities.size();
	entities.push_back(newEntity);
	return handle;
}

void Lina_ECS::RemoveEntity(Lina_EntityHandle * handle)
{
	Entity& entity = HandleToEntity(handle);

	// Remove components
	for (uint32 i = 0; i < entity.size(); i++)
		RemoveComponentInternal(entity[i].first, entity[i].second);

	uint32 destinationIndex = HandleToEntityIndex(handle);
	uint32 sourceIndex = entities.size() - 1;

	// Delete the entity.
	delete entities[destinationIndex];

	// Move the last entity to the deleted entity's position.
	entities[destinationIndex] = entities[sourceIndex];
	
	// Remove last, which is the one we wanted to deleted just now..
	entities.pop_back();
}



void Lina_ECS::GetComponent(Lina_EntityHandle * entity)
{

}

void Lina_ECS::AddSystem(Lina_ECSBaseSystem & system)
{
	systems.push_back(&system);
}

void Lina_ECS::UpdateSystems(float delta)
{
}

void Lina_ECS::RemoveSystems(Lina_ECSBaseSystem & system)
{

}


void Lina_ECS::AddComponentInternal(Entity & entity, uint32 componentID, Lina_ECSBaseComponent * component)
{
	// Get the functor of the particular component.
	Lina_ECSComponentCreateFunction createfn = Lina_ECSBaseComponent::GetTypeCreateFunction(componentID);

	ComponentPair newPair;

	// Init pair.
	newPair.first = componentID;

	/* TODO: FIX HANDLE CAST */
	//newPair.second = createfn(components[componentID], handle, component);

	// Add entity.
	entity.push_back(newPair);
}


void Lina_ECS::DeleteComponent(uint32 componentID, uint32 index)
{
	Lina_DSArray<uint8>& arr = components[componentID];
	Lina_ECSComponentFreeFunction freefn = Lina_ECSBaseComponent::GetTypeFreeFunction(componentID);
	size_t typeSize = Lina_ECSBaseComponent::GetTypeSize(componentID);
	uint32 sourceIndex = arr.size() - typeSize;

	Lina_ECSBaseComponent* baseComp = (Lina_ECSBaseComponent*)&arr[sourceIndex];
	Lina_ECSBaseComponent* destinationComp = (Lina_ECSBaseComponent*)&arr[index];
	freefn(destinationComp);

	// If we are removing the final element of the array.
	if (index == sourceIndex)
	{
		arr.resize(sourceIndex);
		return;
	}

	// Update the entities that are referring to the deleted components.
	/* TODO: INTEGRATE MATH LIB AND DO A MEM CPY */
}