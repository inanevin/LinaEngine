/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Core/PhysicsCommon.hpp"
#include "Core/Backend/PhysX/PhysXPhysicsEngine.hpp"  
#include "Log/Log.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Math/Color.hpp"

namespace Lina::Physics
{
	PhysXPhysicsEngine* PhysXPhysicsEngine::s_physicsEngine;

	PhysXPhysicsEngine::PhysXPhysicsEngine()
	{
		LINA_TRACE("[Constructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	PhysXPhysicsEngine::~PhysXPhysicsEngine()
	{
		LINA_TRACE("[Destructor] -> Physics Engine ({0})", typeid(*this).name());

		
	}

	void PhysXPhysicsEngine::Initialize(Lina::ApplicationMode appMode)
	{
		LINA_TRACE("[Initialization] -> Physics Engine ({0})", typeid(*this).name());
		m_appMode = appMode;
		m_ecs = Lina::ECS::Registry::Get();

		if (m_appMode == Lina::ApplicationMode::Editor)
			SetDebugDraw(true);

		
		m_ecs->on_destroy<ECS::PhysicsComponent>().connect<&PhysXPhysicsEngine::OnPhysicsComponentRemoved>(this);

		// Setup rigidbody system and listen to events so that we can refresh bodies when new rigidbodies are created, destroyed etc.
		m_rigidbodySystem.Initialize();
		m_physicsPipeline.AddSystem(m_rigidbodySystem);

		// Engine events.
		m_eventSystem = Event::EventSystem::Get();
		m_eventSystem->Connect<Event::EPostSceneDraw, &PhysXPhysicsEngine::OnPostSceneDraw>(this);
	}


	void PhysXPhysicsEngine::Tick(float fixedDelta)
	{
		// Update phy.
	
		m_physicsPipeline.UpdateSystems(fixedDelta);
	}

	void PhysXPhysicsEngine::Shutdown()
	{
		LINA_TRACE("[Shutdown] -> Physics Engine ({0})", typeid(*this).name());
	}

	void PhysXPhysicsEngine::OnPostSceneDraw(Event::EPostSceneDraw)
	{
		
	}

	void PhysXPhysicsEngine::SetBodySimulation(ECS::Entity body, bool simulate)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);

	}

	void PhysXPhysicsEngine::SetBodyCollisionShape(ECS::Entity body, Physics::CollisionShape shape)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
	
	}

	void PhysXPhysicsEngine::SetBodyMass(ECS::Entity body, float mass)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		

	}

	void PhysXPhysicsEngine::SetBodyRadius(ECS::Entity body, float radius)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
	
	}

	void PhysXPhysicsEngine::SetBodyHeight(ECS::Entity body, float height)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		
	}

	void PhysXPhysicsEngine::SetBodyHalfExtents(ECS::Entity body, const Vector3& extents)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
	
	}


	void PhysXPhysicsEngine::OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent)
	{
		
	}

	void PhysXPhysicsEngine::RemoveBodyFromWorld(ECS::Entity body)
	{
		
	}

	void PhysXPhysicsEngine::AddBodyToWorld(ECS::Entity body)
	{
		
	}

}

