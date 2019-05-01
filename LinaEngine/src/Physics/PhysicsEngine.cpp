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
Timestamp: 5/1/2019 2:35:43 AM

*/

#include "LinaPch.hpp"
#include "Physics/PhysicsEngine.hpp"  
#include "ECS/EntityComponentSystem.hpp"

namespace LinaEngine::Physics
{
	PhysicsEngine::PhysicsEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	PhysicsEngine::~PhysicsEngine()
	{
		LINA_CORE_TRACE("[Destructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	void PhysicsEngine::Initialize(EntityComponentSystem* ecsIn)
	{
		LINA_CORE_TRACE("[Initialization] -> Physics Engine ({0})", typeid(*this).name());

		// Set ECS reference.
		ECS = ecsIn;

		// Create the interaction world.
		m_InteractionWorld = std::make_unique<PhysicsInteractionWorld>(*ECS);

		// Add interaction world as a listener to ECS.
		ECS->AddListener(m_InteractionWorld.get());
	}

	void PhysicsEngine::Tick(float fixedDelta)
	{
		m_InteractionWorld->Tick(fixedDelta);
	}

	void PhysicsEngine::CleanUp()
	{

	}
}

