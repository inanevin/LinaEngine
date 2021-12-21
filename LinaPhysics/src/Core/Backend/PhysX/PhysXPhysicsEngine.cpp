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
#include "Physics/PhysicsMaterial.hpp"
#include "PxPhysicsAPI.h"

namespace Lina::Physics
{
	PhysXPhysicsEngine* PhysXPhysicsEngine::s_physicsEngine;
	using namespace physx;

	PxDefaultAllocator m_pxAllocator;
	PxDefaultErrorCallback	m_pxErrorCallback;
	PxReal m_pxStackZ = 10.0f;
	physx::PxFoundation* m_pxFoundation = nullptr;
	physx::PxPhysics* m_pxPhysics = nullptr;
	physx::PxDefaultCpuDispatcher* m_pxDispatcher = nullptr;
	physx::PxScene* m_pxScene = nullptr;
	physx::PxMaterial* m_pxDefaultMaterial = nullptr;
	physx::PxPvd* m_pxPvd = nullptr;
	std::map<ECS::Entity, physx::PxRigidDynamic*> m_actors;


	PhysXPhysicsEngine::PhysXPhysicsEngine()
	{
		LINA_TRACE("[Constructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	PhysXPhysicsEngine::~PhysXPhysicsEngine()
	{
		LINA_TRACE("[Destructor] -> Physics Engine ({0})", typeid(*this).name());

		for (auto ent : m_actors)
			ent.second->release();

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
		m_pxScene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);

		if (m_appMode == ApplicationMode::Editor)
		{
			m_pxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			m_pxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.0f);
			m_pxScene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 2.0f);
			m_pxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);

		}

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
		m_eventSystem->Connect<Event::ELoadResourceFromFile, &PhysXPhysicsEngine::OnResourceLoadedFromFile>(this);
		m_eventSystem->Connect<Event::ELoadResourceFromMemory, &PhysXPhysicsEngine::OnResourceLoadedFromMemory>(this);
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
		const PxRenderBuffer& rb = m_pxScene->getRenderBuffer();
		for (PxU32 i = 0; i < rb.getNbLines(); i++)
		{
			const PxDebugLine& line = rb.getLines()[i];
			Color renderColor;
			renderColor.r = (line.color0 >> 16) & 0xff;
			renderColor.g = (line.color0 >> 8) & 0xff;
			renderColor.b = (line.color0 & 0xff);
			renderColor.a = 1.0f;
			m_eventSystem->Trigger<Event::EDrawPhysicsDebug>(Event::EDrawPhysicsDebug{ ToLinaVector3(line.pos0), ToLinaVector3(line.pos1), renderColor });
		}
	}

	PxShape* PhysXPhysicsEngine::GetCreateShape(ECS::PhysicsComponent& phy)
	{
		const CollisionShape shape = phy.GetCollisionShape();
		if (shape == CollisionShape::Box)
			return m_pxPhysics->createShape(PxBoxGeometry(ToPxVector3(phy.GetHalfExtents())), *m_pxDefaultMaterial);
		else if (shape == CollisionShape::Sphere)
			return m_pxPhysics->createShape(PxSphereGeometry(phy.GetRadius()), *m_pxDefaultMaterial);
		else if (shape == CollisionShape::Capsule || shape == CollisionShape::Cylinder)
			return m_pxPhysics->createShape(PxCapsuleGeometry(phy.GetRadius(), phy.GetCapsuleHeight()), *m_pxDefaultMaterial);
		else
			return m_pxPhysics->createShape(PxBoxGeometry(ToPxVector3(phy.GetHalfExtents())), *m_pxDefaultMaterial);

	}

	void PhysXPhysicsEngine::SetBodySimulation(ECS::Entity body, bool simulate)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);

		if (simulate && !phy.m_wasSimulated)
		{
			phy.m_isSimulated = simulate;
			phy.m_wasSimulated = true;
			AddBodyToWorld(body);
		}
		else if (!simulate && phy.m_wasSimulated)
		{
			phy.m_isSimulated = simulate;
			phy.m_wasSimulated = false;
			RemoveBodyFromWorld(body);
		}

		int nb = (int)m_pxScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
	}

	void PhysXPhysicsEngine::SetBodyCollisionShape(ECS::Entity body, Physics::CollisionShape shape)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);

	}

	void PhysXPhysicsEngine::SetBodyMass(ECS::Entity body, float mass)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
	}

	void PhysXPhysicsEngine::SetBodyMaterial(ECS::Entity body, const PhysicsMaterial& material)
	{

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
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_isKinematic = kinematic;

		if (phy.m_isSimulated)
		{
			m_actors[body]->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);

			if (!kinematic)
				m_actors[body]->wakeUp();

		}
	}

	physx::PxActor** PhysXPhysicsEngine::GetActiveActors(uint32& size)
	{
		return m_pxScene->getActiveActors(size);
	}

	ECS::Entity PhysXPhysicsEngine::GetEntityOfActor(physx::PxActor* actor)
	{
		for (auto& p : m_actors)
		{
			if (p.second == actor)
				return p.first;
		}

		return entt::null;
	}

	std::map<ECS::Entity, physx::PxRigidDynamic*>& PhysXPhysicsEngine::GetAllDynamicActors()
	{
		return m_actors;
	}



	void PhysXPhysicsEngine::OnResourceLoadedFromFile(Event::ELoadResourceFromFile ev)
	{
		if (ev.m_resourceType == Resources::ResourceType::PhysicsMaterial)
		{
			LINA_TRACE("[Physics Loader] -> Loading (file): {0}", ev.m_path);
			PhysicsMaterial::LoadMaterialFromFile(ev.m_path);
		}
	}

	void PhysXPhysicsEngine::OnResourceLoadedFromMemory(Event::ELoadResourceFromMemory ev)
	{
		if (ev.m_resourceType == Resources::ResourceType::PhysicsMaterial)
		{
			LINA_TRACE("[Physics Loader] -> Loading (memory): {0}", ev.m_path);
			PhysicsMaterial::LoadMaterialFromMemory(ev.m_path, ev.m_data, ev.m_dataSize);
		}
	}

	void PhysXPhysicsEngine::OnLevelInitialized(Event::ELevelInitialized ev)
	{
		auto& physicsMat = PhysicsMaterial::GetMaterial("Resources/Engine/Physics/Materials/DefaultPhysicsMaterial.phymat");
		m_pxDefaultMaterial->setStaticFriction(physicsMat.m_staticFriction);
		m_pxDefaultMaterial->setDynamicFriction(physicsMat.m_dynamicFriction);
		m_pxDefaultMaterial->setRestitution(physicsMat.m_restitution);

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
		RemoveBodyFromWorld(ent);
	}

	void PhysXPhysicsEngine::RemoveBodyFromWorld(ECS::Entity body)
	{
		m_actors[body]->release();
		m_actors.erase(body);
	}

	void PhysXPhysicsEngine::AddBodyToWorld(ECS::Entity body)
	{
		if (m_actors.find(body) != m_actors.end()) return;

		ECS::EntityDataComponent& data = m_ecs->get<ECS::EntityDataComponent>(body);
		ECS::PhysicsComponent& phyComp = m_ecs->get<ECS::PhysicsComponent>(body);
		PxTransform pose;
		pose.p = ToPxVector3(data.GetLocation());
		pose.q = ToPxQuat(data.GetRotation());
		PxRigidDynamic* rigid = m_pxPhysics->createRigidDynamic(pose);
		PxShape* shape = GetCreateShape(phyComp);
		rigid->attachShape(*shape);
		physx::PxRigidBodyExt::updateMassAndInertia(*rigid, 10.0f);
		shape->release();
		rigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, phyComp.GetIsKinematic());
		m_actors[body] = rigid;
		m_pxScene->addActor(*rigid);
	}

}

