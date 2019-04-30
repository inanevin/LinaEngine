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
Timestamp: 4/30/2019 7:43:50 PM

*/

#include "LinaPch.hpp"
#include "Core/InteractionWorld.hpp"  
#include "Core/STLImport.hpp"

namespace LinaEngine
{

	void InteractionWorld::OnMakeEntity(EntityHandle handle)
	{
		entities.push_back(handle);
	}

	void InteractionWorld::OnRemoveEntity(EntityHandle handle)
	{
		entityDump.push_back(handle);
	}

	void InteractionWorld::OnAddComponent(EntityHandle handle, uint32 id)
	{
		if (id == TransformComponent::ID)
		{
			if (ecs.GetComponent<ColliderComponent>(handle) != nullptr)
			{
				entities.push_back(handle);
			}
		}

		if (id == ColliderComponent::ID)
		{
			if (ecs.GetComponent<TransformComponent>(handle) != nullptr)
			{
				entities.push_back(handle);
			}
		}
	}

	void InteractionWorld::OnRemoveComponent(EntityHandle handle, uint32 id)
	{
		if (id == TransformComponent::ID || id == ColliderComponent::ID)
			entityDump.push_back(handle);
	}

	void InteractionWorld::Tick(float delta)
	{
		// Clear entity dump
		ClearEntityDump();

		// Sort the entities based on their min extends.
		std::sort(entities.begin(), entities.end(), aabbComparator);
		
		// Test the interactions
		Vector3F centerSum(0.0f), centerSqrtSum(0.0f);
		for (size_t i = 0; i < entities.size(); i++)
		{
			AABB aabb = ecs.GetComponent<ColliderComponent>(entities[i])->aabb;

			Vector3F center = aabb.GetCenter();
			centerSum += center;
			centerSqrtSum += (center * center);

			// Find the interactions
			for (size_t j = i + 1; j < entities.size(); j++)
			{
				AABB otherAABB = ecs.GetComponent<ColliderComponent>(entities[j])->aabb;

				// If condition fails, no other entity can possible be interacting w/ the current one
				if (otherAABB.GetMinExtents()[aabbComparator.axis] > aabb.GetMaxExtents()[aabbComparator.axis])
					break;

				if (aabb.DoesIntersect(otherAABB))
				{
					// Interaction rules.
				}
			}
		}

		// Find the max variance. TODO: This part comes 1 update call delayed, does it worth to take it into a pre-update queue?
		centerSum /= entities.size();
		centerSqrtSum /= entities.size();
		
		// Mean(sqrt) - means^2
		Vector3F variance = centerSqrtSum - (centerSum * centerSum);

		// Find max variance component.
		uint32 maxVarianceAxis = 0;
		float maxVariance = variance[0];
		if (variance[1] > maxVariance)
		{
			maxVariance = variance[1];
			maxVarianceAxis = 1;
		}
		if (variance[2] > maxVariance)
		{
			maxVariance = variance[2];
			maxVarianceAxis = 2;
		}

		aabbComparator.axis = maxVarianceAxis;
	}

	void InteractionWorld::ClearEntityDump()
	{

		if (entityDump.size() == 0) return;

		// Check each entity if they have been placed into the dump
		for (size_t i = 0; i < entities.size(); i++)
		{
			bool isRemoved = false;
			do
			{
				isRemoved = false;
				for (size_t j = 0; j < entityDump.size(); j++)
				{
					if (entities[i] == entityDump[j])
					{
						// put the entity at the end of the list & pop it.
						entities.swap_remove(i);
						entityDump.swap_remove(j);
						isRemoved = true;
						break;
					}
				}

				if (isRemoved && entityDump.size() == 0) return;
			} while (isRemoved);
		}

		entityDump.clear();
	}

}

