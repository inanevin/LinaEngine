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
Timestamp: 9/30/2020 2:54:13 AM

*/

#include "ECS/Systems/RigidbodySystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/RigidbodyComponent.hpp"
#include "Physics/PhysicsEngine.hpp"

namespace LinaEngine::ECS
{
	void RigidbodySystem::UpdateComponents(float delta)
	{
		auto view = m_Registry->view<TransformComponent, RigidbodyComponent>();


		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			RigidbodyComponent& rbComponent = view.get<RigidbodyComponent>(entity);
			btRigidBody* rb = m_physicsEngine->GetActiveRigidbody(rbComponent.m_bodyID);

			// Get transform.
			btTransform btTrans;
			rb->getMotionState()->getWorldTransform(btTrans);

			// Set location
			transform.transform.m_location.x = btTrans.getOrigin().getX();
			transform.transform.m_location.y = btTrans.getOrigin().getY();
			transform.transform.m_location.z = btTrans.getOrigin().getZ();

			// Set rotation
			transform.transform.m_rotation.x = btTrans.getRotation().getX();
			transform.transform.m_rotation.y = btTrans.getRotation().getY();
			transform.transform.m_rotation.z = btTrans.getRotation().getZ();
			transform.transform.m_rotation.w = btTrans.getRotation().getW();
		}
	}
}


