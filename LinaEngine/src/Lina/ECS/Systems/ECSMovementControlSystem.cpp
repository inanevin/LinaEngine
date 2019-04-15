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
#include "ECSMovementControlSystem.hpp"  
#include "Lina/Input/InputAxisBinder.hpp"

using namespace LinaEngine::Input;

namespace LinaEngine::ECS
{
	void ECSMovementControlSystem::UpdateComponents(float delta, BaseECSComponent ** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
		MovementControlComponent* movementControl = (MovementControlComponent*)components[1];

		for (uint32 i = 0; i < movementControl->movementControls.size(); i++)
		{
			Vector3F movement = movementControl->movementControls[i].first;
			InputKeyAxisBinder* input = movementControl->movementControls[i].second;
			Vector3F newPos = transform->transform.GetPosition() + (movement * input->GetAmount() * delta);
			transform->transform.SetPosition(newPos);
		}

		
	}
}

