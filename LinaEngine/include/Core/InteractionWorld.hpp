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
Timestamp: 4/30/2019 7:43:36 PM

*/

#pragma once

#ifndef InteractionWorld_HPP
#define InteractionWorld_HPP

#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/ColliderComponent.hpp"

using namespace LinaEngine::ECS;

namespace LinaEngine
{



	class Interaction
	{
	public:

		virtual void Interact(float delta, BaseECSComponent** interactorComponents, BaseECSComponent** interacteeComponents) {};

		FORCEINLINE const LinaArray<uint32>& GetInteractorComponents() { return interactorComponentTypes; }
		FORCEINLINE const LinaArray<uint32>& GetInteracteeComponents() { return interacteeComponentTypes; }

	protected:

		FORCEINLINE void AddInteractorComponentType(uint32 type)
		{
			interactorComponentTypes.push_back(type);
		}

		FORCEINLINE void AddInteracteeComponentType(uint32 type)
		{
			interacteeComponentTypes.push_back(type);
		}

	private:

		LinaArray<uint32> interactorComponentTypes;
		LinaArray<uint32> interacteeComponentTypes;
	};

	class InteractionWorld : public ECSListener
	{
	public:

		InteractionWorld(EntityComponentSystem& ecsIn) : ECSListener(), ecs(ecsIn), aabbComparator(ecsIn, 0)
		{
			AddComponentID(TransformComponent::ID);
			AddComponentID(ColliderComponent::ID);
		}

		virtual void OnMakeEntity(EntityHandle handle) override;
		virtual void OnRemoveEntity(EntityHandle handle) override;
		virtual void OnAddComponent(EntityHandle handle, uint32 id) override;
		virtual void OnRemoveComponent(EntityHandle handle, uint32 id) override;

		// Processes the interactions.
		void Tick(float delta);

		// Add a new interaction.
		FORCEINLINE void AddInteraction(Interaction* interaction)
		{
			interactions.push_back(interaction);
		}

	private:

		struct EntityInternal
		{
			EntityHandle handle;
			LinaArray<uint32> interactors;
			LinaArray<uint32> interactees;
		};

		struct InteractionWorldCompare
		{
			uint32 axis = 0;
			EntityComponentSystem& ecs;

			InteractionWorldCompare(EntityComponentSystem& ecsIn, uint32 axisIn) : ecs(ecsIn), axis(axisIn) {};

			bool operator()(EntityInternal a, EntityInternal b)
			{
				float aMin = ecs.GetComponent<ColliderComponent>(a.handle)->aabb.GetMinExtents()[axis];
				float bMin = ecs.GetComponent<ColliderComponent>(b.handle)->aabb.GetMinExtents()[axis];
				return (aMin < bMin);
			}
		};

	private:
		
		// Removes all entities in the dump.
		void ClearEntityDump();

		// Adds an entity to the list.
		void AddEntity(EntityHandle handle);

		// Computes specific interactions
		void ComputeInteractions(EntityInternal& entity, uint32 interactionIndex);

	private:

		EntityComponentSystem& ecs;
		LinaArray<Interaction*> interactions;
		LinaArray<EntityInternal> entities;
		LinaArray<EntityHandle> entityDump;
		InteractionWorldCompare aabbComparator;
	};
}


#endif