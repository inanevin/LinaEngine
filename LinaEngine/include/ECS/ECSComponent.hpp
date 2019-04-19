/*
Author: Inan Evin - Thanks to the lectures & contributions of Benny Bobaganoosh, thebennybox.
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: ECSComponent
Timestamp: 4/7/2019 3:24:08 PM

*/

#pragma once

#ifndef ECSComponent_HPP
#define ECSComponent_HPP

#include "LinaPch.hpp"
#include "Core/DataStructures.hpp"

namespace LinaEngine::ECS
{

	/* Forward definition for base ecs component. */
	struct BaseECSComponent;

	/* We don't have to know about the entity, we just keep a void pointer. */
	typedef void* EntityHandle;

	/* Defines for create & free functions */
	typedef uint32 (*ECSComponentCreateFunction)(LinaArray<uint8>& memory, EntityHandle entity, BaseECSComponent* comp);
	typedef void (*ECSComponentFreeFunction)(BaseECSComponent* component);

	/* Null pointer for entitites */
#define NULL_ENTITY_HANDLE nullptr

	/* Base strcut for EntityComponentSystem components. */
	struct BaseECSComponent
	{

		/* Increments id for new components */
		static uint32 registerComponentType(ECSComponentCreateFunction createfn, ECSComponentFreeFunction freefn, size_t size);

		/* Entity reference */
		EntityHandle entity = NULL_ENTITY_HANDLE;

		FORCEINLINE static ECSComponentCreateFunction GetTypeCreateFunction(uint32 id)
		{
			return std::get<0>((*componentTypes)[id]);
		}

		FORCEINLINE static ECSComponentFreeFunction GetTypeFreeFunction(uint32 id)
		{
			return std::get<1>((*componentTypes)[id]);
		}

		FORCEINLINE static size_t GetTypeSize(uint32 id)
		{
			return std::get<2>((*componentTypes)[id]);
		}

		FORCEINLINE static bool isTypeValid(uint32 id)
		{
			return id < componentTypes->size();
		}

	private:
		/* Component types, warning = global dynamic mem alloc. */
		static LinaArray<LinaTuple<ECSComponentCreateFunction, ECSComponentFreeFunction, size_t>>* componentTypes;
	};

	template<typename T>
	struct ECSComponent : public BaseECSComponent
	{

		/* Create function for creating ecs components. */
		static const ECSComponentCreateFunction CREATE_FUNCTION;

		/* Free function for freeing an ecs component. */
		static const ECSComponentFreeFunction FREE_FUNCTION;

		/* Component ID */
		static const uint32 ID;

		/* How big every component is */
		static const size_t SIZE;
	};

	/* Creates a component from a base reference */
	template<typename Component>
	uint32 ECSComponentCreate(LinaArray<uint8>& memory, EntityHandle entity, BaseECSComponent* comp)
	{
		uint32 index = memory.size();
		memory.resize(index + Component::SIZE);
		Component* component = new(&memory[index]) Component(*(Component*)comp);
		component->entity = entity;
		return index;
	}

	/* Frees a particular component */
	template<typename Component>
	void ECSComponentFree(BaseECSComponent* comp)
	{
		Component* component = (Component*)comp;
		component->~Component();
	}

	/* Define ID for mid ECSComponent class. */
	template<typename T>
	const uint32 ECSComponent<T>::ID(BaseECSComponent::registerComponentType(ECSComponentCreate<T>, ECSComponentFree<T>, sizeof(T)));

	/* Define SIZE for mid ECSComponent class. */
	template<typename T>
	const size_t ECSComponent<T>::SIZE(sizeof(T));

	/* Def create func */
	template<typename T>
	const ECSComponentCreateFunction ECSComponent<T>::CREATE_FUNCTION(ECSComponentCreate<T>);

	/* Def free func */
	template<typename T>
	const ECSComponentFreeFunction ECSComponent<T>::FREE_FUNCTION(ECSComponentFree<T>);


	struct TestComponent : public ECSComponent<TestComponent>
	{
		float x;
		float y;
	};
}


#endif