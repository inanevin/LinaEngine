/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RigidbodySystem
Timestamp: 9/30/2020 2:52:58 AM

*/
#pragma once

#ifndef RigidbodySystem_HPP
#define RigidbodySystem_HPP

// Headers here.
#include "ECS/ECS.hpp"

namespace LinaEngine
{
	namespace Physics
	{
		class PhysicsEngine;
	}
}

namespace LinaEngine::ECS
{
	class RigidbodySystem : public BaseECSSystem
	{
	public:

		RigidbodySystem() {};
		virtual void UpdateComponents(float delta) override;

		// Construct the system.
		FORCEINLINE void Construct(ECSRegistry& registry, LinaEngine::Physics::PhysicsEngine* physicsEngine) { BaseECSSystem::Construct(registry); m_physicsEngine = physicsEngine; }



	private:

		LinaEngine::Physics::PhysicsEngine* m_physicsEngine = nullptr;

	};
}

#endif
