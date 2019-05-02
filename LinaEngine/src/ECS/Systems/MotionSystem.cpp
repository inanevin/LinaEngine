/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: MotionSystem
Timestamp: 4/28/2019 3:38:12 AM

*/

#include "LinaPch.hpp"
#include "ECS/Systems/MotionSystem.hpp"  
#include "ECS/Utility/MotionIntegrators.hpp"

namespace LinaEngine::ECS
{
	static float deltaIncrement = 0.0f;


		void MotionSystem::UpdateComponents(float delta, BaseECSComponent ** components)
		{
			TransformComponent* transform = (TransformComponent*)components[0];
			MotionComponent* motion = (MotionComponent*)components[1];

			deltaIncrement += delta / 100.0f;
			Vector3F newPos = transform->transform.GetTranslation();
			ModifiedEuler(newPos, motion->velocity, motion->acceleration, delta);
			transform->transform.SetTranslation(newPos);

		}

}

