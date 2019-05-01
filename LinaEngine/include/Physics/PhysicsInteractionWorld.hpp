/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: PhysicsInteractionWorld
Timestamp: 5/1/2019 2:42:57 AM

*/

#pragma once

#ifndef PhysicsInteractionWorld_HPP
#define PhysicsInteractionWorld_HPP

#include "ECS/EntityComponentSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/ColliderComponent.hpp"
#include "ECS/ECSInteraction.hpp"

using namespace LinaEngine::ECS;


namespace LinaEngine::Physics
{
	class PhysicsInteractionWorld : public ECSListener
	{
	public:

		PhysicsInteractionWorld(EntityComponentSystem& ecsIn) : ECSListener(), ecs(ecsIn), aabbComparator(ecsIn, 0)
		{
			// Notify all component operations, but not all entity operations.
			SetNotificationParams(true, false);
			AddComponentID(TransformComponent::ID);
			AddComponentID(ColliderComponent::ID);
			LINA_CORE_TRACE("[Constructor] -> Physics Interaction World ({0})", typeid(*this).name());

		}

		~PhysicsInteractionWorld()
		{
			LINA_CORE_TRACE("[Destructor] -> Physics Interaction World ({0})", typeid(*this).name());
		}

		virtual void OnMakeEntity(EntityHandle handle) override;
		virtual void OnRemoveEntity(EntityHandle handle) override;
		virtual void OnAddComponent(EntityHandle handle, uint32 id) override;
		virtual void OnRemoveComponent(EntityHandle handle, uint32 id) override;

		// Processes the interactions.
		void Tick(float delta);

		// Add a new interaction.
		void AddInteraction(ECSInteraction* interaction);

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
		void ClearEntityDumpAndUpdate();

		// Adds an entity to the list.
		void AddEntity(EntityHandle handle);

		// Computes specific interactions
		void ComputeInteractions(EntityInternal& entity, uint32 interactionIndex);

	private:

		EntityComponentSystem& ecs;
		LinaArray<ECSInteraction*> interactions;
		LinaArray<EntityInternal> entities;
		LinaArray<EntityHandle> entityDump;
		LinaArray<EntityHandle> entityUpdateStack;
		InteractionWorldCompare aabbComparator;
	};
}


#endif