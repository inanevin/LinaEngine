/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ECSMovementControlSystem
Timestamp: 4/9/2019 3:30:12 PM

*/

#include "LinaPch.hpp"
#include "ECS/Systems/MovementControlSystem.hpp"  
#include "ECS/Components/MotionComponent.hpp"
#include "ECS/Components/MovementControlComponent.hpp"
#include "Input/InputAxisBinder.hpp"


namespace LinaEngine::ECS
{
	MovementControlSystem::MovementControlSystem() : BaseECSSystem()
	{
		AddComponentType(MovementControlComponent::ID);
		AddComponentType(MotionComponent::ID);
	}
	void MovementControlSystem::UpdateComponents(float delta, BaseECSComponent ** components)
	{
		MovementControlComponent* movementControl = (MovementControlComponent*)components[0];
		MotionComponent* motionComponent = (MotionComponent*)components[1];

		for (uint32 i = 0; i < movementControl->movementControls.size(); i++)
		{
			Vector3F movement = movementControl->movementControls[i].movement;
			float inputAmt = movementControl->movementControls[i].inputKeyAxisBinder->GetAmount();
			float inputDelta = inputAmt - movementControl->movementControls[i].lastInputAmount;
			motionComponent->acceleration += movement * inputDelta;
			movementControl->movementControls[i].lastInputAmount = inputAmt;
		}

		// Limit speed TODO: Add resistance later on.
		motionComponent->velocity *= 0.95f;

		
	}
}

