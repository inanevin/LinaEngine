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
10/24/2018 1:09:41 AM

*/

#pragma once

#ifndef Lina_ECS_HPP
#define Lina_ECS_HPP

#include "ECS/Lina_ECSComponent.hpp"
#include "ECS/Lina_ECSSystem.hpp"
#include <map>

typedef std::pair<uint32, uint32> ComponentPair;
typedef Lina_DSArray<ComponentPair> Entity;
typedef std::pair<uint32, Entity> IndexEntityPair;

class Lina_ECS
{
public:
	Lina_ECS() {}
	~Lina_ECS();

	// Entity methods
	EntityHandle MakeEntity(Lina_ECSBaseComponent* components, const uint32* componentIDs, size_t numComponents);
	void removeEntity(EntityHandle handle);

	// Component methods
	template<class Component>
	inline void AddComponent(EntityHandle entity, Component* component)
	{
		AddComponentInternal(entity, HandleToEntity(entity), Component::ID, component);
	}

	template<class Component>
	bool RemoveComponent(EntityHandle entity)
	{
		return RemoveComponentInternal(entity, Component::ID);
	}

	template<class Component>
	Component* GetComponent(EntityHandle entity)
	{
		GetComponentInternal(HandleToEntity(entity), Component::ID);
	}

	// System methods
	inline void AddSystem(Lina_ECSBaseSystem& system)
	{
		systems.push_back(&system);
	}
	void UpdateSystems(float delta);
	void RemoveSystems(Lina_ECSBaseSystem& system);
private:
	Lina_DSArray<Lina_ECSBaseSystem*> systems;
	std::map<uint32, Lina_DSArray<uint8>> components;
	Lina_DSArray<std::pair<uint32, Lina_DSArray<std::pair<uint32, uint32> > >* > entities;

	inline std::pair<uint32, Lina_DSArray<std::pair<uint32, uint32> > >* HandleToRaw(EntityHandle handle)
	{
		return (std::pair<uint32, Lina_DSArray<std::pair<uint32, uint32> > >*)handle;
	}

	inline uint32 HandleToEntityIndex(EntityHandle handle)
	{
		return HandleToRaw(handle)->first;
	}

	inline Lina_DSArray<std::pair<uint32, uint32> >& HandleToEntity(EntityHandle handle)
	{
		return HandleToRaw(handle)->second;
	}

	void DeleteComponent(uint32 componentID, uint32 index);
	bool RemoveComponentInternal(EntityHandle handle, uint32 componentID);
	void AddComponentInternal(EntityHandle handle, Lina_DSArray<std::pair<uint32, uint32> >& entity, uint32 componentID, Lina_ECSBaseComponent* component);
	Lina_ECSBaseComponent* GetComponentInternal(Lina_DSArray<std::pair<uint32, uint32> >& entityComponents, uint32 componentID);

	NULL_COPY_AND_ASSIGN(Lina_ECS);
};

#endif

