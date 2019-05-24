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

	float constVelocity;

	void CubeChunkSystem::UpdateComponents(float delta, BaseECSComponent ** components)
	{
		CubeChunkComponent* component = (CubeChunkComponent*)components[0];

		if (m_MovementIndex == 0)
		{
			if (component->indexChanged)
			{
				Vector3F v = Vector3F(Math::RandF()*10.0f - 5.0f, Math::RandF()*10.0f - 5.0f,
					Math::RandF()*10.0f - 5.0f + 20.0f)
					;

				float vf = -4.0f;
				float af = 5.0f;
				Vector3F v2 = Vector3F(Math::RandF(-af, af), Math::RandF(-af, af), Math::RandF(-af, af));

				component->indexChanged = false;
				component->position[0] = v.GetX();
				component->position[1] = v.GetY();
				component->position[2] = v.GetZ();
		
				component->acceleration[0] = v2.GetX();
				component->acceleration[1] = v2.GetY();
				component->acceleration[2] = v2.GetZ();

				component->velocity[0] = component->acceleration[0] * vf;
				component->velocity[1] = component->acceleration[1] * vf;
				component->velocity[2] = component->acceleration[2] * vf;
			}

		
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

			constVelocity = 0.0f;
		}
		else if(m_MovementIndex == 1)
		{
			component->indexChanged = true;

			Vector3F newPosition(component->position[0], component->position[1], component->position[2]);
			Vector3F newVelocity(component->velocity[0], component->velocity[1], component->velocity[2]);
			Vector3F acceleration(component->acceleration[0], component->acceleration[1], component->acceleration[2]);

			constVelocity += delta * 10.0f;

			newPosition = Vector3F(Math::RandF() * 200, 10.0f, Math::RandF() * 200);
			newVelocity = Vector3F(0.0f, constVelocity, 0.0f);

			component->position[0] = newPosition[0];
			component->position[1] = newPosition[1];
			component->position[2] = newPosition[2];

			component->velocity[0] = newVelocity[0];
			component->velocity[1] = newVelocity[1];
			component->velocity[2] = newVelocity[2];

			component->acceleration[1] = constVelocity;
		}

	}
}

