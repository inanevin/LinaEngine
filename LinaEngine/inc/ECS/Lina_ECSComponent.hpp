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
10/23/2018 10:19:33 PM

*/

#pragma once

#ifndef Lina_ECSComponent_HPP
#define Lina_ECSComponent_HPP

#include <tuple>
#include "DataStructures/Lina_DSArray.hpp"
#include "Utility/Lina_Common.hpp"

/*
#define NULL_ENTITY_HANDLE nullptr
struct Lina_ECSBaseComponent;
typedef void* Lina_EntityHandle;
typedef uint32 (*Lina_ECSComponentCreateFunction)(Lina_DSArray<uint8_t>& memory, Lina_EntityHandle entity, Lina_ECSBaseComponent* comp);
typedef void (*Lina_ECSComponentFreeFunction)(Lina_ECSBaseComponent* comp);*/


struct Lina_ECSBaseComponent;
typedef void* EntityHandle;
typedef uint32(*Lina_ECSComponentCreateFunction)(Lina_DSArray<uint8>& memory, EntityHandle entity, Lina_ECSBaseComponent* comp);
typedef void(*Lina_ECSComponentFreeFunction)(Lina_ECSBaseComponent* comp);
#define NULL_ENTITY_HANDLE nullptr

struct Lina_ECSBaseComponent
{
public:
	static uint32 RegisterComponentType(Lina_ECSComponentCreateFunction createfn,
		Lina_ECSComponentFreeFunction freefn, size_t size);
	EntityHandle entity = NULL_ENTITY_HANDLE;

	inline static Lina_ECSComponentCreateFunction GetTypeCreateFunction(uint32 id)
	{
		return std::get<0>(componentTypes[id]);
	}

	inline static Lina_ECSComponentFreeFunction GetTypeFreeFunction(uint32 id)
	{
		return std::get<1>(componentTypes[id]);
	}

	inline static size_t GetTypeSize(uint32 id)
	{
		return std::get<2>(componentTypes[id]);
	}

	inline static bool IsTypeValid(uint32 id)
	{
		return id < componentTypes.size();
	}
private:
	static Lina_DSArray<std::tuple<Lina_ECSComponentCreateFunction, Lina_ECSComponentFreeFunction, size_t> > componentTypes;
};

template<typename T>
struct Lina_ECSComponent : public Lina_ECSBaseComponent
{
	static const Lina_ECSComponentCreateFunction CREATE_FUNCTION;
	static const Lina_ECSComponentFreeFunction FREE_FUNCTION;
	static const uint32 ID;
	static const size_t SIZE;
};

template<typename Component>
uint32 Lina_ECSComponentCreate(Lina_DSArray<uint8>& memory, EntityHandle entity, Lina_ECSBaseComponent* comp)
{
	uint32 index = memory.size();
	memory.resize(index + Component::SIZE);
	Component* component = new(&memory[index])Component(*(Component*)comp);
	component->entity = entity;
	return index;
}

template<typename Component>
void Lina_ECSComponentFree(Lina_ECSBaseComponent* comp)
{
	Component* component = (Component*)comp;
	component->~Component();
}

template<typename T>
const uint32 Lina_ECSComponent<T>::ID(Lina_ECSBaseComponent::RegisterComponentType(Lina_ECSComponentCreate<T>, Lina_ECSComponentFree<T>, sizeof(T)));

template<typename T>
const size_t Lina_ECSComponent<T>::SIZE(sizeof(T));

template<typename T>
const Lina_ECSComponentCreateFunction Lina_ECSComponent<T>::CREATE_FUNCTION(Lina_ECSComponentCreate<T>);

template<typename T>
const Lina_ECSComponentFreeFunction Lina_ECSComponent<T>::FREE_FUNCTION(Lina_ECSComponentFree<T>);

//BEGIN EXAMPLE CODE
struct TestComponent : public Lina_ECSComponent<TestComponent>
{
	float x;
	float y;
};

#endif