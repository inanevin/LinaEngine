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
#include "Math/Math.hpp"
#include "Physics/Raycast.hpp"
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
	std::map<ECS::Entity, physx::PxRigidDynamic*> m_dynamicActors;
	std::map<ECS::Entity, physx::PxRigidStatic*> m_staticActors;
	std::map<StringIDType, physx::PxMaterial*> m_materials;

	PhysXPhysicsEngine::PhysXPhysicsEngine()
	{
		LINA_TRACE("[Constructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	PhysXPhysicsEngine::~PhysXPhysicsEngine()
	{
		LINA_TRACE("[Destructor] -> Physics Engine ({0})", typeid(*this).name());

		for (auto ent : m_staticActors)
			ent.second->release();

		for (auto ent : m_dynamicActors)
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
		m_ecs->on_construct<ECS::PhysicsComponent>().connect<&PhysXPhysicsEngine::OnPhysicsComponentAdded>(this);
		m_ecs->on_construct<ECS::EntityDataComponent>().connect<&PhysXPhysicsEngine::OnPhysicsComponentAdded>(this);

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

		static float x = 0.0f;

		x += fixedDelta;

		if (x > 2.0f)
		{
			x = 0.0f;

			PxRaycastHit hitInfo;
			PxU32 maxHits = 1;
			PxTransform pose;
			pose.p = PxVec3(0, 0, 0);
			pose.q = ToPxQuat(Quaternion(Vector3(0,0,1), 0));

			HitInfo h = RaycastPose(Vector3(0, 0, -10), Vector3(0, 0, 1), Vector3(5, 0, 0), Vector3(5,5,5), 100);

			LINA_TRACE("Raycast {0}", h.m_hitCount);

			//PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;
			//PxU32 hitCount = PxGeometryQuery::raycast(PxVec3(0,0, -10), PxVec3(0,0,1),
			//	PxBoxGeometry(), pose,
			//	100,
			//	hitFlags,
			//	maxHits, &hitInfo);
			//
			//
			//if (hitInfo.actor != nullptr)
			//	LINA_TRACE("actor aint null");
			//
			//LINA_TRACE("RAY CAST {0} pos: {1}", hitCount, ToLinaVector3(hitInfo.position).ToString());

		}

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
			m_eventSystem->Trigger<Event::EDrawLine>(Event::EDrawLine{ ToLinaVector3(line.pos0), ToLinaVector3(line.pos1), renderColor });
		}
	}

	PxShape* PhysXPhysicsEngine::GetCreateShape(ECS::PhysicsComponent& phy)
	{
		const CollisionShape shape = phy.GetCollisionShape();

		PxMaterial* mat = nullptr;

		if (m_materials.find(phy.GetMaterialID()) != m_materials.end())
		{
			mat = m_materials[phy.GetMaterialID()];
		}
		else
		{
			auto& phyMat = PhysicsMaterial::GetMaterial(phy.GetMaterialID());
			m_materials[phy.GetMaterialID()] = m_pxPhysics->createMaterial(phyMat.m_staticFriction, phyMat.m_dynamicFriction, phyMat.m_restitution);
		}

		LINA_ASSERT(mat != nullptr, "Physics material is null!");

		if (shape == CollisionShape::Box)
			return m_pxPhysics->createShape(PxBoxGeometry(ToPxVector3(phy.GetHalfExtents())), *mat);
		else if (shape == CollisionShape::Sphere)
			return m_pxPhysics->createShape(PxSphereGeometry(phy.GetRadius()), *m_pxDefaultMaterial);
		else if (shape == CollisionShape::Capsule)
			return m_pxPhysics->createShape(PxCapsuleGeometry(phy.GetRadius(), phy.GetCapsuleHalfHeight()), *mat);
		else
			return m_pxPhysics->createShape(PxBoxGeometry(ToPxVector3(phy.GetHalfExtents())), *mat);

	}

	void PhysXPhysicsEngine::SetMaterialStaticFriction(PhysicsMaterial& mat, float friction)
	{
		StringIDType sid = mat.GetID();
		if (m_materials.find(sid) == m_materials.end())
			m_materials[sid] = m_pxPhysics->createMaterial(mat.m_staticFriction, mat.m_dynamicFriction, mat.m_restitution);

		m_materials[sid]->setStaticFriction(mat.m_staticFriction);
	}

	void PhysXPhysicsEngine::SetMaterialDynamicFriction(PhysicsMaterial& mat, float friction)
	{
		StringIDType sid = mat.GetID();
		if (m_materials.find(sid) == m_materials.end())
			m_materials[sid] = m_pxPhysics->createMaterial(mat.m_staticFriction, mat.m_dynamicFriction, mat.m_restitution);

		m_materials[sid]->setDynamicFriction(mat.m_dynamicFriction);
	}

	void PhysXPhysicsEngine::SetMaterialRestitution(PhysicsMaterial& mat, float restitution)
	{
		StringIDType sid = mat.GetID();
		if (m_materials.find(sid) == m_materials.end())
			m_materials[sid] = m_pxPhysics->createMaterial(mat.m_staticFriction, mat.m_dynamicFriction, mat.m_restitution);

		m_materials[sid]->setRestitution(mat.m_restitution);
	}

	void PhysXPhysicsEngine::SetBodySimulation(ECS::Entity body, SimulationType type)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		auto& data = m_ecs->get<ECS::EntityDataComponent>(body);

		if (type == SimulationType::Static)
			data.m_isTransformLocked = true;
		else
			data.m_isTransformLocked = false;

		SimulationType previousType = phy.m_simType;

		if (previousType == SimulationType::None)
		{
			if (type == SimulationType::Dynamic)
				AddBodyToWorld(body, true);
			else if (type == SimulationType::Static)
				AddBodyToWorld(body, false);
		}
		else if (previousType == SimulationType::Static)
		{
			if (type == SimulationType::None)
				RemoveBodyFromWorld(body, false);
			else if (type == SimulationType::Dynamic)
			{
				RemoveBodyFromWorld(body, false);
				AddBodyToWorld(body, true);
			}
		}
		else if (previousType == SimulationType::Dynamic)
		{
			if (type == SimulationType::None)
				RemoveBodyFromWorld(body, true);
			else if (type == SimulationType::Static)
			{
				RemoveBodyFromWorld(body, true);
				AddBodyToWorld(body, false);
			}
		}

		phy.m_simType = type;
	}

	void PhysXPhysicsEngine::SetBodyCollisionShape(ECS::Entity body, Physics::CollisionShape shape)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_collisionShape = shape;
		UpdateBodyShape(body);
	}

	void PhysXPhysicsEngine::SetBodyMass(ECS::Entity body, float mass)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_mass = Math::Clamp(mass, 0.1f, 1000.0f);

		if (phy.GetSimType() == SimulationType::Dynamic)
			PxRigidBodyExt::updateMassAndInertia(*m_dynamicActors[body], mass);
	}

	void PhysXPhysicsEngine::SetBodyMaterial(ECS::Entity body, const PhysicsMaterial& material)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_physicsMaterialID = material.m_materialID;
		phy.m_physicsMaterialPath = material.m_path;
		UpdateBodyShape(body);
	}

	void PhysXPhysicsEngine::SetBodyRadius(ECS::Entity body, float radius)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_radius = Math::Clamp(radius, 0.1f, 50.0f);
		UpdateBodyShape(body);
	}

	void PhysXPhysicsEngine::SetBodyHeight(ECS::Entity body, float height)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_capsuleHalfHeight = Math::Clamp(height, 0.5f, 100.0f);
		UpdateBodyShape(body);
	}

	void PhysXPhysicsEngine::SetBodyHalfExtents(ECS::Entity body, const Vector3& extents)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_halfExtents = Vector3(Math::Clamp(extents.x, 0.1f, 50.0f), Math::Clamp(extents.y, 0.1f, 50.0f), Math::Clamp(extents.z, 0.1f, 50.0f));
		UpdateBodyShape(body);
	}

	void PhysXPhysicsEngine::OnPhysicsComponentAdded(entt::registry& reg, entt::entity ent)
	{
		auto* phy = m_ecs->try_get<ECS::PhysicsComponent>(ent);
		if (phy == nullptr) return;


		if (phy->m_physicsMaterialPath.compare("") == 0)
		{
			Physics::PhysicsMaterial& mat = Physics::PhysicsMaterial::GetMaterial("Resources/Engine/Physics/Materials/DefaultPhysicsMaterial.phymat");
			phy->m_physicsMaterialID = mat.GetID();
			phy->m_physicsMaterialPath = mat.GetPath();
		}


		auto* data = m_ecs->try_get<ECS::EntityDataComponent>(ent);

		if (data != nullptr)
		{
			if (phy->GetSimType() == SimulationType::Dynamic)
			{
				if (m_dynamicActors.find(ent) == m_dynamicActors.end())
					AddBodyToWorld(ent, true);
			}
			else if (phy->GetSimType() == SimulationType::Static)
			{
				if (m_staticActors.find(ent) == m_staticActors.end())
					AddBodyToWorld(ent, false);
			}
		}


	}

	void PhysXPhysicsEngine::UpdateShapeMaterials(ECS::Entity body, physx::PxShape* shape)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		PhysicsMaterial& mat = PhysicsMaterial::GetMaterial(phy.m_physicsMaterialID);

		PxMaterial* material = nullptr;
		shape->getMaterials(&material, 1);

		if (material == nullptr) return;
		material->setStaticFriction(mat.m_staticFriction);
		material->setDynamicFriction(mat.m_dynamicFriction);
		material->setRestitution(mat.m_restitution);
	}

	void PhysXPhysicsEngine::UpdateBodyShape(ECS::Entity body)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);

		if (phy.m_simType != SimulationType::None)
		{
			PxU32 nbShapes = 0;
			PxShape* currentShape = nullptr;
			PxShape* newShape = GetCreateShape(phy);

			if (phy.m_simType == SimulationType::Dynamic)
			{
				auto* actor = m_dynamicActors[body];
				nbShapes = actor->getNbShapes();
				actor->getShapes(&currentShape, 1);
				actor->detachShape(*currentShape);
				actor->attachShape(*newShape);
			}
			else
			{
				auto* actor = m_staticActors[body];
				nbShapes = actor->getNbShapes();
				actor->getShapes(&currentShape, 1);
				actor->detachShape(*currentShape);
				actor->attachShape(*newShape);
			}

			newShape->release();
		}
	}

	void PhysXPhysicsEngine::SetBodyKinematic(ECS::Entity body, bool kinematic)
	{
		auto& phy = m_ecs->get<ECS::PhysicsComponent>(body);
		phy.m_isKinematic = kinematic;

		if (phy.m_simType == SimulationType::Dynamic)
		{
			m_dynamicActors[body]->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);

			if (!kinematic)
				m_dynamicActors[body]->wakeUp();

		}
	}

	physx::PxActor** PhysXPhysicsEngine::GetActiveActors(uint32& size)
	{
		return m_pxScene->getActiveActors(size);
	}

	ECS::Entity PhysXPhysicsEngine::GetEntityOfActor(physx::PxActor* actor)
	{
		for (auto& p : m_dynamicActors)
		{
			if (p.second == actor)
				return p.first;
		}

		return entt::null;
	}

	std::map<ECS::Entity, physx::PxRigidDynamic*>& PhysXPhysicsEngine::GetAllDynamicActors()
	{
		return m_dynamicActors;
	}

	std::map<ECS::Entity, physx::PxRigidStatic*>& PhysXPhysicsEngine::GetAllStaticActors()
	{
		return m_staticActors;
	}

	std::map<StringIDType, physx::PxMaterial*>& PhysXPhysicsEngine::GetMaterials()
	{
		return m_materials;
	}

	void PhysXPhysicsEngine::OnResourceLoadedFromFile(Event::ELoadResourceFromFile ev)
	{
		if (ev.m_resourceType == Resources::ResourceType::PhysicsMaterial)
		{
			LINA_TRACE("[Physics Loader] -> Loading (file): {0}", ev.m_path);
			auto& mat = PhysicsMaterial::LoadMaterialFromFile(ev.m_path);
			m_materials[mat.GetID()] = m_pxPhysics->createMaterial(mat.m_staticFriction, mat.m_dynamicFriction, mat.m_restitution);
		}
	}

	void PhysXPhysicsEngine::OnResourceLoadedFromMemory(Event::ELoadResourceFromMemory ev)
	{
		if (ev.m_resourceType == Resources::ResourceType::PhysicsMaterial)
		{
			LINA_TRACE("[Physics Loader] -> Loading (memory): {0}", ev.m_path);
			auto& mat = PhysicsMaterial::LoadMaterialFromMemory(ev.m_path, ev.m_data, ev.m_dataSize);
			m_materials[mat.GetID()] = m_pxPhysics->createMaterial(mat.m_staticFriction, mat.m_dynamicFriction, mat.m_restitution);
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

			if (phyComp.m_simType == SimulationType::Dynamic)
				AddBodyToWorld(entity, true);
			else if (phyComp.m_simType == SimulationType::Static)
				AddBodyToWorld(entity, false);
		}
	}

	void PhysXPhysicsEngine::OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent)
	{
		if (m_dynamicActors.find(ent) != m_dynamicActors.end())
			RemoveBodyFromWorld(ent, true);
		else if (m_staticActors.find(ent) != m_staticActors.end())
			RemoveBodyFromWorld(ent, false);
	}

	void PhysXPhysicsEngine::RemoveBodyFromWorld(ECS::Entity body, bool isDynamic)
	{
		if (isDynamic)
		{
			LINA_TRACE("Removing dynamic actor to world. {0}", body);
			m_dynamicActors[body]->release();
			m_dynamicActors.erase(body);
		}
		else
		{
			LINA_TRACE("Removing static actor to world. {0}", body);
			m_staticActors[body]->release();
			m_staticActors.erase(body);
		}
	}

	void PhysXPhysicsEngine::AddBodyToWorld(ECS::Entity body, bool isDynamic)
	{
		if (isDynamic && m_dynamicActors.find(body) != m_dynamicActors.end()) return;
		if (!isDynamic && m_staticActors.find(body) != m_staticActors.end()) return;

		ECS::EntityDataComponent& data = m_ecs->get<ECS::EntityDataComponent>(body);
		ECS::PhysicsComponent& phyComp = m_ecs->get<ECS::PhysicsComponent>(body);
		PxTransform pose;
		pose.p = ToPxVector3(data.GetLocation());
		pose.q = ToPxQuat(data.GetRotation());
		PxShape* shape = GetCreateShape(phyComp);

		if (phyComp.GetCollisionShape() == CollisionShape::Capsule)
		{
			PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
			shape->setLocalPose(relativePose);
		}

		if (isDynamic)
		{
			LINA_TRACE("Adding a dynamic actor to world. {0}", body);
			PxRigidDynamic* rigid = m_pxPhysics->createRigidDynamic(pose);
			rigid->attachShape(*shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*rigid, 10.0f);
			rigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, phyComp.GetIsKinematic());
			m_dynamicActors[body] = rigid;
			m_pxScene->addActor(*rigid);
		}
		else
		{
			LINA_TRACE("Adding a static actor to world. {0}", body);
			PxRigidStatic* stc = m_pxPhysics->createRigidStatic(pose);
			stc->attachShape(*shape);
			m_staticActors[body] = stc;
			m_pxScene->addActor(*stc);
		}

		shape->release();
	}

}

