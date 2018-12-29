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

Lina_ECS::~Lina_ECS()
{
	for (std::map<uint32, Lina_DSArray<uint8>>::iterator it = components.begin(); it != components.end(); ++it) {
		size_t typeSize = Lina_ECSBaseComponent::GetTypeSize(it->first);
		Lina_ECSComponentFreeFunction freefn = Lina_ECSBaseComponent::GetTypeFreeFunction(it->first);
		for (uint32 i = 0; i < it->second.size(); i += typeSize) {
			freefn((Lina_ECSBaseComponent*)&it->second[i]);
		}
	}

	for (uint32 i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
}

EntityHandle Lina_ECS::MakeEntity(Lina_ECSBaseComponent* entityComponents, const uint32* componentIDs, size_t numComponents)
{
	std::pair<uint32, Lina_DSArray<std::pair<uint32, uint32> > >* newEntity = new std::pair<uint32, Lina_DSArray<std::pair<uint32, uint32> > >();
	EntityHandle handle = (EntityHandle)newEntity;
	for (uint32 i = 0; i < numComponents; i++) {
		if (!Lina_ECSBaseComponent::IsTypeValid(componentIDs[i])) {
			DEBUG_LOG("ECS", LOG_ERROR, "'%u' is not a valid component type.", componentIDs[i]);
			delete newEntity;
			return NULL_ENTITY_HANDLE;
		}

		AddComponentInternal(handle, newEntity->second, componentIDs[i], &entityComponents[i]);
	}

	newEntity->first = entities.size();
	entities.push_back(newEntity);
	return handle;
}

void Lina_ECS::removeEntity(EntityHandle handle)
{
	Lina_DSArray<std::pair<uint32, uint32> >& entity = HandleToEntity(handle);
	for (uint32 i = 0; i < entity.size(); i++) {
		DeleteComponent(entity[i].first, entity[i].second);
	}

	uint32 destIndex = HandleToEntityIndex(handle);
	uint32 srcIndex = entities.size() - 1;
	delete entities[destIndex];
	entities[destIndex] = entities[srcIndex];
	entities.pop_back();
}

void Lina_ECS::AddComponentInternal(EntityHandle handle, Lina_DSArray<std::pair<uint32, uint32> >& entity, uint32 componentID, Lina_ECSBaseComponent* component)
{
	Lina_ECSComponentCreateFunction createfn = Lina_ECSBaseComponent::GetTypeCreateFunction(componentID);
	std::pair<uint32, uint32> newPair;
	newPair.first = componentID;
	newPair.second = createfn(components[componentID], handle, component);
	entity.push_back(newPair);
}

void Lina_ECS::DeleteComponent(uint32 componentID, uint32 index)
{
	Lina_DSArray<uint8>& array = components[componentID];
	Lina_ECSComponentFreeFunction freefn = Lina_ECSBaseComponent::GetTypeFreeFunction(componentID);
	size_t typeSize = Lina_ECSBaseComponent::GetTypeSize(componentID);
	uint32 srcIndex = array.size() - typeSize;

	Lina_ECSBaseComponent* destComponent = (Lina_ECSBaseComponent*)&array[index];
	Lina_ECSBaseComponent* srcComponent = (Lina_ECSBaseComponent*)&array[srcIndex];
	freefn(destComponent);

	if (index == srcIndex) {
		array.resize(srcIndex);
		return;
	}
	std::memcpy(destComponent, srcComponent, typeSize);

	Lina_DSArray<std::pair<uint32, uint32> >& srcComponents = HandleToEntity(srcComponent->entity);
	for (uint32 i = 0; i < srcComponents.size(); i++) {
		if (componentID == srcComponents[i].first && srcIndex == srcComponents[i].second) {
			srcComponents[i].second = index;
			break;
		}
	}

	array.resize(srcIndex);
}

bool Lina_ECS::RemoveComponentInternal(EntityHandle handle, uint32 componentID)
{
	Lina_DSArray<std::pair<uint32, uint32> >& entityComponents = HandleToEntity(handle);
	for (uint32 i = 0; i < entityComponents.size(); i++) {
		if (componentID == entityComponents[i].first) {
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

Lina_ECSBaseComponent* Lina_ECS::GetComponentInternal(Lina_DSArray<std::pair<uint32, uint32> >& entityComponents, uint32 componentID)
{
	for (uint32 i = 0; i < entityComponents.size(); i++) {
		if (componentID == entityComponents[i].first) {
			return (Lina_ECSBaseComponent*)&components[componentID][entityComponents[i].second];
		}
	}
	return nullptr;
}