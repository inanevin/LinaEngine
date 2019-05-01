/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: PhysicsEngine
Timestamp: 5/1/2019 2:35:28 AM

*/

#pragma once

#ifndef PhysicsEngine_HPP
#define PhysicsEngine_HPP

#include "Physics/PhysicsInteractionWorld.hpp"

namespace LinaEngine::ECS
{
	class EntityComponentSystem;
}

using namespace LinaEngine::ECS;

namespace LinaEngine::Physics
{

	class PhysicsEngine
	{
	public:

		PhysicsEngine();
		~PhysicsEngine();

		// Initializes the physics engine.
		void Initialize(EntityComponentSystem* ecsIn);

		// Update physics systems.
		void Tick(float fixedDelta);

		// Termination.
		void CleanUp();

	private:

		// Interaction world that handles physics interactions.
		std::unique_ptr<PhysicsInteractionWorld> m_InteractionWorld;

		// ECS reference.
		EntityComponentSystem* ECS;

	};
}


#endif