/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: CubeChunkSystem
Timestamp: 4/30/2019 12:05:30 AM

*/

#include "LinaPch.hpp"
#include "ECS/Systems/CubeChunkSystem.hpp"  
#include "ECS/Utility/MotionIntegrators.hpp"

namespace LinaEngine::ECS
{
	void CubeChunkSystem::UpdateComponents(float delta, BaseECSComponent ** components)
	{
		CubeChunkComponent* component = (CubeChunkComponent*)components[0];

		Vector3F newPosition(component->position[0], component->position[1], component->position[2]);
		Vector3F newVelocity(component->velocity[0], component->velocity[1], component->velocity[2]);
		Vector3F acceleration(component->acceleration[0], component->acceleration[1], component->acceleration[2]);
		ForestRuth(newPosition, newVelocity, acceleration, delta);

		component->position[0] = newPosition[0];
		component->position[1] = newPosition[1];
		component->position[2] = newPosition[2];

		component->velocity[0] = newVelocity[0];
		component->velocity[1] = newVelocity[1];
		component->velocity[2] = newVelocity[2];

	}
}

