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
#include "PxPhysicsAPI.h"

namespace Lina::Physics
{
	PhysXPhysicsEngine* PhysXPhysicsEngine::s_physicsEngine;

	using namespace physx;

	PxDefaultAllocator m_pxAllocator;
	PxDefaultErrorCallback	m_pxErrorCallback;
	PxFoundation* m_pxFoundation = NULL;
	PxPhysics* m_pxPhysics = NULL;
	PxDefaultCpuDispatcher* m_pxDispatcher = NULL;
	PxScene* m_pxScene = NULL;
	PxMaterial* m_pxDefaultMaterial = NULL;
	PxPvd* m_pxPvd = NULL;
	PxReal m_pxStackZ = 10.0f;

	PhysXPhysicsEngine::PhysXPhysicsEngine()
	{
		LINA_TRACE("[Constructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	PhysXPhysicsEngine::~PhysXPhysicsEngine()
	{
		LINA_TRACE("[Destructor] -> Physics Engine ({0})", typeid(*this).name());

		m_pxScene->release();
		m_pxDispatcher->release();
		m_pxPhysics->release();

		if (m_pxPvd)
		{
			PxPvdTransport* transport = m_pxPvd->getTransport();
			m_pxPvd->release();
			m_pxPvd = NULL;
			transport->release();
		}

		m_pxFoundation->release();
	}

	void PhysXPhysicsEngine::Initialize(Lina::ApplicationMode appMode)
	{
		LINA_TRACE("[Initialization] -> Physics Engine ({0})", typeid(*this).name());
		m_appMode = appMode;
		m_ecs = Lina::ECS::Registry::Get();

		if (m_appMode == Lina::ApplicationMode::Editor)
			SetDebugDraw(true);

		m_pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_pxAllocator, m_pxErrorCallback);
		LINA_ASSERT(m_pxFoundation != nullptr, "Nvidia PhysX foundation could not be created!");

		m_pxPvd = PxCreatePvd(*m_pxFoundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(nullptr, 5425, 10);
		m_pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		m_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pxFoundation, PxTolerancesScale(), true, m_pxPvd);
		LINA_ASSERT(m_pxPhysics != nullptr, "Nvidia PhysX could not be initialized!");

		PxSceneDesc sceneDesc(m_pxPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		m_pxDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = m_pxDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		m_pxScene = m_pxPhysics->createScene(sceneDesc);

		PxPvdSceneClient* pvdClient = m_pxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		m_pxDefaultMaterial = m_pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);


		m_ecs->on_destroy<ECS::PhysicsComponent>().connect<&PhysXPhysicsEngine::OnPhysicsComponentRemoved>(this);

		// Setup rigidbody system and listen to events so that we can refresh bodies when new rigidbodies are created, destroyed etc.
		m_rigidbodySystem.Initialize();
		m_physicsPipeline.AddSystem(m_rigidbodySystem);

		// Engine events.
		m_eventSystem = Event::EventSystem::Get();
		m_eventSystem->Connect<Event::EPostSceneDraw, &PhysXPhysicsEngine::OnPostSceneDraw>(this);
		m_eventSystem->Connect<Event::ELevelInitialized, &PhysXPhysicsEngine::OnLevelInitialized>(this);
	}


	void PhysXPhysicsEngine::Tick(float fixedDelta)
	{
		// Update phy.

		m_pxScene->simulate(PHYSICS_STEP);
		m_pxScene->fetchResults(true);
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

	void PhysXPhysicsEngine::SetBodyKinematic(ECS::Entity body, bool kinematic)
	{

	}


	void PhysXPhysicsEngine::OnLevelInitialized(Event::ELevelInitialized ev)
	{
		auto view = m_ecs->view<ECS::PhysicsComponent>();

		for (auto entity : view)
		{
			ECS::PhysicsComponent& phyComp = view.get<ECS::PhysicsComponent>(entity);

			if (phyComp.m_isSimulated)
				AddBodyToWorld(entity);
		}
	}

	void PhysXPhysicsEngine::OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent)
	{

	}

	void PhysXPhysicsEngine::RemoveBodyFromWorld(ECS::Entity body)
	{

	}

	void PhysXPhysicsEngine::AddBodyToWorld(ECS::Entity body)
	{
		ECS::EntityDataComponent& data = m_ecs->get<ECS::EntityDataComponent>(body);
		ECS::PhysicsComponent& phyComp = m_ecs->get<ECS::PhysicsComponent>(body);
		PxTransform pose;
		pose.p = ToPxVector3(data.GetLocation());
		pose.q = ToPxQuat(data.GetRotation());
		PxRigidDynamic* rigid = m_pxPhysics->createRigidDynamic(pose);
		
		PxShape* shape = m_pxPhysics->createShape()
		rigid->attachShape(shape);
	}

}

