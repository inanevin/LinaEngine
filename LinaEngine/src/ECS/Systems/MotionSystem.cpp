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

namespace LinaEngine::ECS
{
	
		void verlet(LinaEngine::Vector3F& pos, LinaEngine::Vector3F& velocity, const LinaEngine::Vector3F& acceleration, float delta)
		{
			float halfDelta = delta * 0.5f;
			pos += velocity * halfDelta;
			velocity += acceleration * delta;
			pos += velocity * halfDelta;
		}

		void forestRuth(LinaEngine::Vector3F& pos, LinaEngine::Vector3F& velocity, const LinaEngine::Vector3F& acceleration, float delta)
		{
			static const float frCoefficient = 1.0f / (2.0f - LinaEngine::Math::Pow(2.0f, 1.0f / 3.0f));
			static const float frComplement = 1.0f - 2.0f*frCoefficient;
			verlet(pos, velocity, acceleration, delta*frCoefficient);
			verlet(pos, velocity, acceleration, delta*frComplement);
			verlet(pos, velocity, acceleration, delta*frCoefficient);
		}

		void modifiedEuler(LinaEngine::Vector3F& pos, LinaEngine::Vector3F& velocity, const LinaEngine::Vector3F& acceleration, float delta)
		{
			velocity += acceleration * delta;
			pos += velocity * delta;
		}
	

		void MotionSystem::UpdateComponents(float delta, BaseECSComponent ** components)
		{
			TransformComponent* transform = (TransformComponent*)components[0];
			MotionComponent* motion = (MotionComponent*)components[1];

			Vector3F newPos = transform->transform.GetTranslation();
			forestRuth(newPos, motion->velocity, motion->acceleration, delta);
			transform->transform.SetTranslation(newPos);
		}

}

