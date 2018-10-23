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

typedef std::pair<uint32, uint32> componentPair;
typedef Lina_DSArray<componentPair> entityData;
typedef std::pair<uint32, entityData> entityPairs;

class Lina_ECS
{

public:

	Lina_ECS() {};
	~Lina_ECS();

	/* ENTITY FUNCTIONS */
	Lina_EntityHandle MakeEntity(Lina_ECSBaseComponent* components, const uint32* componentIDs, size_t componentCount);
	void RemoveEntity(Lina_EntityHandle* handle);

	/* COMPONENT FUNCTIONS */

	template<class Component>
	void AddComponent(Lina_EntityHandle* entity, Component* component);

	template<class Component>
	void RemoveComponent(Lina_EntityHandle* entity);

	void GetComponent(Lina_EntityHandle* entity);

	/* SYSTEM FUNCTIONS */
	void AddSystem(Lina_ECSBaseSystem& system)
	{
		systems.push_back(&system);
	}

	void UpdateSystems(float delta);

	void RemoveSystems(Lina_ECSBaseSystem& system);

private:

	Lina_DSArray<Lina_ECSBaseSystem*> systems;
	std::map<uint32, Lina_DSArray<uint8>> components;
	Lina_DSArray <entityPairs*> entities;

	inline entityPairs* HandleToRawType(Lina_EntityHandle* handle)
	{
		(entityPairs*)handle;
	}

	inline uint32 HandleToEntityIndex(Lina_EntityHandle* handle)
	{
		return HandleToRawType(handle)->first;
	}

	inline entityData& HandleToEntity(Lina_EntityHandle* handle)
	{
		return HandleToRawType(handle)->second;
	}

	NULL_COPY_AND_ASSIGN(Lina_ECS);
};


#endif