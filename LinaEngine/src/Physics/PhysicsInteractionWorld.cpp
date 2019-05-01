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
Timestamp: 5/1/2019 2:43:16 AM

*/

#include "LinaPch.hpp"
#include "Physics/PhysicsInteractionWorld.hpp"  
#include "Core/STLImport.hpp"

namespace LinaEngine::Physics
{
	void PhysicsInteractionWorld::OnMakeEntity(EntityHandle handle)
	{
		AddEntity(handle);
	}

	void PhysicsInteractionWorld::OnRemoveEntity(EntityHandle handle)
	{
		entityDump.push_back(handle);
	}

	void PhysicsInteractionWorld::OnAddComponent(EntityHandle handle, uint32 id)
	{
		if (id == TransformComponent::ID)
		{
			if (ecs.GetComponent<ColliderComponent>(handle) != nullptr)
				AddEntity(handle);
		}
		else if (id == ColliderComponent::ID)
		{
			if (ecs.GetComponent<TransformComponent>(handle) != nullptr)
				AddEntity(handle);
		}
		else if (ecs.GetComponent<ColliderComponent>(handle) != nullptr && ecs.GetComponent<TransformComponent>(handle) != nullptr)
		{
			entityUpdateStack.push_back(handle);
		}

	}

	void PhysicsInteractionWorld::OnRemoveComponent(EntityHandle handle, uint32 id)
	{
		if (id == TransformComponent::ID || id == ColliderComponent::ID)
			entityDump.push_back(handle);
		else if (ecs.GetComponent<ColliderComponent>(handle) != nullptr && ecs.GetComponent<TransformComponent>(handle) != nullptr)
			entityUpdateStack.push_back(handle);
	}

	void PhysicsInteractionWorld::Tick(float delta)
	{
		// Clear entity dump
		ClearEntityDumpAndUpdate();

		// Sort the entities based on their min extends.
		std::sort(entities.begin(), entities.end(), aabbComparator);

		// Arrays for components if interactions match
		LinaArray<BaseECSComponent*> interactorComponents;
		LinaArray<BaseECSComponent*> interacteeComponents;

		// Test the interactions
		Vector3F centerSum(0.0f), centerSqrtSum(0.0f);
		for (size_t i = 0; i < entities.size(); i++)
		{
			AABB aabb = ecs.GetComponent<ColliderComponent>(entities[i].handle)->aabb;

			Vector3F center = aabb.GetCenter();
			centerSum += center;
			centerSqrtSum += (center * center);

			// Find the interactions
			for (size_t j = i + 1; j < entities.size(); j++)
			{
				AABB otherAABB = ecs.GetComponent<ColliderComponent>(entities[j].handle)->aabb;

				// If condition fails, no other entity can possible be interacting w/ the current one
				if (otherAABB.GetMinExtents()[aabbComparator.axis] > aabb.GetMaxExtents()[aabbComparator.axis])
					break;

				if (aabb.Intersects(otherAABB))
				{
					size_t interactorIndex = i;
					size_t interacteeIndex = j;

					// Run the check twice. One for entity being the interactor, check against the interactees.
					// Another one for entity being the interactee, check against the interactors.
					for (size_t entityInteractionSwitchIndex = 0; entityInteractionSwitchIndex < 2; entityInteractionSwitchIndex++)
					{
						for (size_t k = 0; k < entities[interactorIndex].interactors.size(); k++)
						{
							for (size_t l = 0; l < entities[interacteeIndex].interactees.size(); l++)
							{
								uint32 index = entities[interactorIndex].interactors[k];
								// Interaction is valid!
								if (index == entities[interacteeIndex].interactees[l])
								{
									// Get the interaction
									ECSInteraction* interaction = interactions[index];

									// Make sure we have enough space in our arrays.
									interactorComponents.resize(Math::Max(interactorComponents.size(), interaction->GetInteractorComponents().size()));
									interacteeComponents.resize(Math::Max(interactorComponents.size(), interaction->GetInteractorComponents().size()));

									// Set interactor components.
									for (size_t m = 0; m < interaction->GetInteractorComponents().size(); m++)
										interactorComponents[m] = ecs.GetComponentByType(entities[interactorIndex].handle, interaction->GetInteractorComponents()[m]);

									// Set interactee components.
									for (size_t m = 0; m < interaction->GetInteractorComponents().size(); m++)
										interactorComponents[m] = ecs.GetComponentByType(entities[interacteeIndex].handle, interaction->GetInteracteeComponents()[m]);

									// Finally interact
									interaction->Interact(delta, &interactorComponents[0], &interacteeComponents[0]);
								}
							}
						}

						// Swap interactee / interactor check.
						size_t tempIndex = interactorIndex;
						interactorIndex = interacteeIndex;
						interacteeIndex = tempIndex;
					}
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

	void PhysicsInteractionWorld::AddInteraction(ECSInteraction * interaction)
	{
		interactions.push_back(interaction);
		// Compute each interaction.
		for (size_t i = 0; i < entities.size(); i++)
			ComputeInteractions(entities[i], interactions.size() - 1);
	}

	void PhysicsInteractionWorld::ClearEntityDumpAndUpdate()
	{
		if (entityDump.size() == 0) return;

		// Iterate through entities.
		for (size_t i = 0; i < entities.size(); i++)
		{

			// Check the entity if it has been placed into the dump for removal.
			bool isRemoved = false;
			do
			{
				isRemoved = false;
				for (size_t j = 0; j < entityDump.size(); j++)
				{
					if (entities[i].handle == entityDump[j])
					{
						// put the entity at the end of the list & pop it.
						entities.swap_remove(i);
						entityDump.swap_remove(j);
						isRemoved = true;
						break;
					}
				}

				if (entityDump.size() == 0 && entityUpdateStack.size() == 0) return;

			} while (isRemoved);

			for (size_t j = 0; j < entityUpdateStack.size(); j++)
			{
				// We need to update this one.
				if (entities[i].handle == entityUpdateStack[j])
				{
					entities[i].interactees.clear();
					entities[i].interactors.clear();

					// re-compute interactions for this entity.
					for (size_t k = 0; k < interactions.size(); k++)
						ComputeInteractions(entities[i], k);

					entityUpdateStack.swap_remove(j);
				}
			}
		}

		entityDump.clear();
		entityUpdateStack.clear();
	}

	void PhysicsInteractionWorld::AddEntity(EntityHandle handle)
	{
		EntityInternal entity;
		entity.handle = handle;

		// Compute each interaction.
		for (size_t i = 0; i < interactions.size(); i++)
			ComputeInteractions(entity, i);

		entities.push_back(entity);
	}

	void PhysicsInteractionWorld::ComputeInteractions(EntityInternal & entity, uint32 interactionIndex)
	{
		ECSInteraction* interaction = interactions[interactionIndex];

		// Check if this entity is an interactor
		bool isInteractor = true;
		for (size_t i = 0; i < interaction->GetInteractorComponents().size(); i++)
		{
			if (ecs.GetComponentByType(entity.handle, interaction->GetInteractorComponents()[i]) == nullptr)
			{
				isInteractor = false;
				break;
			}
		}

		// Check if this entity is an interactee
		bool isInteractee = true;
		for (size_t i = 0; i < interaction->GetInteracteeComponents().size(); i++)
		{
			if (ecs.GetComponentByType(entity.handle, interaction->GetInteracteeComponents()[i]) == nullptr)
			{
				isInteractee = false;
				break;
			}
		}

		// Add to the corresponding list if valid.
		if (isInteractor)
			entity.interactors.push_back(interactionIndex);

		if (isInteractee)
			entity.interactees.push_back(interactionIndex);
	}
}

