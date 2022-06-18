/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Core/PhysicsEngine.hpp"

#include "Core/PhysicsCommon.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Registry.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/ECSEvents.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Resources/ResourceStorage.hpp"
#include "Physics/PhysicsMaterial.hpp"
#include "Physics/Raycast.hpp"
#include <PxPhysicsAPI.h>
#include <cereal/archives/portable_binary.hpp>
#include <fstream>

namespace Lina::Physics
{
    PhysicsEngine* PhysicsEngine::s_physicsEngine;
    using namespace physx;

    PxDefaultAllocator      m_pxAllocator;
    PxDefaultErrorCallback  m_pxErrorCallback;
    PxReal                  m_pxStackZ          = 10.0f;
    PxFoundation*           m_pxFoundation      = nullptr;
    PxPhysics*              m_pxPhysics         = nullptr;
    PxDefaultCpuDispatcher* m_pxDispatcher      = nullptr;
    PxScene*                m_pxScene           = nullptr;
    PxMaterial*             m_pxDefaultMaterial = nullptr;
    PxPvd*                  m_pxPvd             = nullptr;

    std::map<ECS::Entity, physx::PxRigidActor*> m_actors;
    std::map<StringIDType, physx::PxMaterial*>  m_materials;
    std::map<ECS::Entity, PxShape*>             m_shapes;

    // Key is the target model, value is a vector of pairs whose key is the node ID in the model and value is the cooked mesh.
    std::map<StringIDType, std::vector<std::pair<int, PxConvexMesh*>>> m_convexMeshMap;

    void PhysicsEngine::Initialize(ApplicationMode appMode)
    {
        LINA_TRACE("[Initialization] -> Physics Engine ({0})", typeid(*this).name());
        m_appMode = appMode;

        if (m_appMode == ApplicationMode::Editor)
            SetDebugDraw(true);

        m_pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_pxAllocator, m_pxErrorCallback);
        LINA_ASSERT(m_pxFoundation != nullptr, "Nvidia PhysX foundation could not be created!");

        m_pxPvd                   = PxCreatePvd(*m_pxFoundation);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(nullptr, 5425, 10);
        m_pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

        m_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pxFoundation, PxTolerancesScale(), true, m_pxPvd);
        LINA_ASSERT(m_pxPhysics != nullptr, "Nvidia PhysX could not be initialized!");

        PxSceneDesc sceneDesc(m_pxPhysics->getTolerancesScale());
        sceneDesc.gravity       = PxVec3(0.0f, -9.81f, 0.0f);
        m_pxDispatcher          = PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = m_pxDispatcher;
        sceneDesc.filterShader  = PxDefaultSimulationFilterShader;
        m_pxScene               = m_pxPhysics->createScene(sceneDesc);
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

        // Setup rigidbody system and listen to events so that we can refresh bodies when new rigidbodies are created, destroyed etc.
        m_rigidbodySystem.Initialize("Rigidbody System", this);
        m_physicsPipeline.AddSystem(m_rigidbodySystem);

        // Engine events.
        m_eventSystem = Event::EventSystem::Get();
        m_eventSystem->Connect<Event::EPostSceneDraw, &PhysicsEngine::OnPostSceneDraw>(this);
        m_eventSystem->Connect<Event::ELevelInstalled, &PhysicsEngine::OnLevelInstalled>(this);
        m_eventSystem->Connect<Event::EEntityEnabledChanged, &PhysicsEngine::OnEntityEnabledChanged>(this);

        m_cooker.Initialize(m_appMode, m_pxFoundation);
    }

    void PhysicsEngine::Tick(float fixedDelta)
    {
        // Update phy.
        m_pxScene->simulate((PxReal)m_stepTime);
        m_pxScene->fetchResults(true);
        m_physicsPipeline.UpdateSystems(fixedDelta);
    }

    void PhysicsEngine::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Physics Engine ({0})", typeid(*this).name());

        m_cooker.Shutdown();

        for (auto ent : m_actors)
            ent.second->release();

        for (auto v : m_convexMeshMap)
        {
            for (uint32 i = 0; i < v.second.size(); i++)
                v.second[i].second->release();
            v.second.clear();
        }

        m_convexMeshMap.clear();
        m_actors.clear();
        m_shapes.clear();

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

    void PhysicsEngine::OnPostSceneDraw(const Event::EPostSceneDraw&)
    {
        const PxRenderBuffer& rb = m_pxScene->getRenderBuffer();
        for (PxU32 i = 0; i < rb.getNbLines(); i++)
        {
            const PxDebugLine& line = rb.getLines()[i];
            Color              renderColor;
            renderColor.r = (float)((line.color0 >> 16) & 0xff);
            renderColor.g = (float)((line.color0 >> 8) & 0xff);
            renderColor.b = (float)((line.color0 & 0xff));
            renderColor.a = 1.0f;
            m_eventSystem->Trigger<Event::EDrawLine>(Event::EDrawLine{ToLinaVector3(line.pos0), ToLinaVector3(line.pos1), renderColor});
        }
    }

    PxShape* PhysicsEngine::GetCreateShape(ECS::PhysicsComponent& phy, ECS::Entity ent)
    {
        const CollisionShape shape = phy.GetCollisionShape();

        PxMaterial* mat = nullptr;

        if (m_materials.find(phy.m_material.m_sid) != m_materials.end())
            mat = m_materials[phy.m_material.m_sid];
        else
        {
            auto* phyMat                      = phy.m_material.m_value;
            m_materials[phy.m_material.m_sid] = m_pxPhysics->createMaterial(phyMat->m_staticFriction, phyMat->m_dynamicFriction, phyMat->m_restitution);
            mat                               = m_materials[phy.m_material.m_sid];
        }

        LINA_ASSERT(mat != nullptr, "Physics material is null!");

        if (shape == CollisionShape::Box)
            return m_pxPhysics->createShape(PxBoxGeometry(ToPxVector3(phy.GetHalfExtents())), *mat, true);
        else if (shape == CollisionShape::Sphere)
            return m_pxPhysics->createShape(PxSphereGeometry(phy.GetRadius()), *m_pxDefaultMaterial, true);
        else if (shape == CollisionShape::Capsule)
        {
            PxShape*    newShape = m_pxPhysics->createShape(PxCapsuleGeometry(phy.GetRadius(), phy.GetCapsuleHalfHeight()), *mat, true);
            PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
            newShape->setLocalPose(relativePose);
            return newShape;
        }
        else if (shape == CollisionShape::ConvexMesh)
        {
            // auto* mr = ECS::Registry::Get()->try_get<ECS::MeshRendererComponent>(ent);
            //
            // if (mr != nullptr)
            // {
            // 	// mr->
            //
            // }
            return m_pxPhysics->createShape(PxConvexMeshGeometry(m_convexMeshMap[phy.m_attachedModelID][0].second, PxMeshScale(PxVec3(100.0f, 100.0f, 100.0f))), *mat, true);
        }

        return m_pxPhysics->createShape(PxBoxGeometry(ToPxVector3(phy.GetHalfExtents())), *mat);
    }

    void PhysicsEngine::CreateConvexMesh(std::vector<uint8>& data, StringIDType sid, int nodeID)
    {
        PxDefaultMemoryInputData input(&data[0], (PxU32)data.size());
        m_convexMeshMap[sid].push_back(std::make_pair(nodeID, m_pxPhysics->createConvexMesh(input)));

        LINA_TRACE("Created convex mesh with sid {0} and nodeID {1}", sid, nodeID);
    }

    bool PhysicsEngine::IsEntityAPhysicsActor(ECS::Entity ent)
    {
        return m_actors.find(ent) != m_actors.end();
    }

    void PhysicsEngine::AddToPhysicsPipeline(ECS::System& system)
    {
        m_physicsPipeline.AddSystem(system);
    }

    void PhysicsEngine::SetMaterialStaticFriction(PhysicsMaterial& mat, float friction)
    {
        StringIDType sid = mat.GetSID();
        if (m_materials.find(sid) == m_materials.end())
            m_materials[sid] = m_pxPhysics->createMaterial(mat.m_staticFriction, mat.m_dynamicFriction, mat.m_restitution);

        m_materials[sid]->setStaticFriction(mat.m_staticFriction);
    }

    void PhysicsEngine::SetMaterialDynamicFriction(PhysicsMaterial& mat, float friction)
    {
        StringIDType sid = mat.GetSID();
        if (m_materials.find(sid) == m_materials.end())
            m_materials[sid] = m_pxPhysics->createMaterial(mat.m_staticFriction, mat.m_dynamicFriction, mat.m_restitution);

        m_materials[sid]->setDynamicFriction(mat.m_dynamicFriction);
    }

    void PhysicsEngine::SetMaterialRestitution(PhysicsMaterial& mat, float restitution)
    {
        StringIDType sid = mat.GetSID();
        if (m_materials.find(sid) == m_materials.end())
            m_materials[sid] = m_pxPhysics->createMaterial(mat.m_staticFriction, mat.m_dynamicFriction, mat.m_restitution);

        m_materials[sid]->setRestitution(mat.m_restitution);
    }

    void PhysicsEngine::SetBodySimulation(ECS::Entity body, SimulationType type)
    {
        auto& phy  = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        auto& data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(body);

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
                RemoveBodyFromWorld(body);
            else if (type == SimulationType::Dynamic)
            {
                RemoveBodyFromWorld(body);
                AddBodyToWorld(body, true);
            }
        }
        else if (previousType == SimulationType::Dynamic)
        {
            if (type == SimulationType::None)
                RemoveBodyFromWorld(body);
            else if (type == SimulationType::Static)
            {
                RemoveBodyFromWorld(body);
                AddBodyToWorld(body, false);
            }
        }

        phy.m_simType = type;
    }

    void PhysicsEngine::SetBodyCollisionShape(ECS::Entity body, Physics::CollisionShape shape)
    {
        auto& phy            = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        phy.m_collisionShape = shape;
        RecreateBodyShape(body);
    }

    void PhysicsEngine::SetBodyMass(ECS::Entity body, float mass)
    {
        auto& phy  = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        phy.m_mass = Math::Clamp(mass, 0.1f, 1000.0f);
        if (phy.GetSimType() == SimulationType::Dynamic)
            PxRigidBodyExt::updateMassAndInertia(*(PxRigidDynamic*)m_actors[body], phy.m_mass);
    }

    void PhysicsEngine::SetBodyMaterial(ECS::Entity body, PhysicsMaterial* material)
    {
        if (!IsEntityAPhysicsActor(body))
            return;

        auto& phy              = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        phy.m_material.m_sid   = material->GetSID();
        phy.m_material.m_value = material;

        PxMaterial* pxMaterial = nullptr;
        m_shapes[body]->getMaterials(&pxMaterial, 1);

        if (pxMaterial == nullptr)
            return;
        pxMaterial->setStaticFriction(material->m_staticFriction);
        pxMaterial->setDynamicFriction(material->m_dynamicFriction);
        pxMaterial->setRestitution(material->m_restitution);
    }

    void PhysicsEngine::SetBodyRadius(ECS::Entity body, float radius)
    {
        auto& phy    = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        phy.m_radius = Math::Clamp(radius, 0.1f, 50.0f);
        UpdateBodyShapeParameters(body);
    }

    void PhysicsEngine::SetBodyHeight(ECS::Entity body, float height)
    {
        auto& phy               = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        phy.m_capsuleHalfHeight = Math::Clamp(height, 0.5f, 100.0f);
        UpdateBodyShapeParameters(body);
    }

    void PhysicsEngine::SetBodyHalfExtents(ECS::Entity body, const Vector3& extents)
    {
        auto& phy         = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        phy.m_halfExtents = Vector3(Math::Clamp(extents.x, 0.1f, 50.0f), Math::Clamp(extents.y, 0.1f, 50.0f), Math::Clamp(extents.z, 0.1f, 50.0f));
        UpdateBodyShapeParameters(body);
    }

    void PhysicsEngine::OnPhysicsComponentAdded(entt::registry& reg, entt::entity ent)
    {
        auto* phy = ECS::Registry::Get()->try_get<ECS::PhysicsComponent>(ent);
        if (phy == nullptr)
            return;

        if (phy->m_material.m_value == nullptr)
        {
            phy->m_material.m_value = m_defaultMaterial;
            phy->m_material.m_sid   = phy->m_material.m_value->GetSID();
        }

        auto* data = ECS::Registry::Get()->try_get<ECS::EntityDataComponent>(ent);
        if (phy->GetSimType() == SimulationType::None)
            return;

        if (data != nullptr && m_actors.find(ent) == m_actors.end())
            AddBodyToWorld(ent, phy->GetSimType() == SimulationType::Dynamic);
    }

    void PhysicsEngine::UpdateBodyShapeParameters(ECS::Entity body)
    {
        auto& phy = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        if (phy.m_simType == SimulationType::None)
            return;

        PxShape* shape = m_shapes[body];
        auto&    data  = ECS::Registry::Get()->get<ECS::EntityDataComponent>(body);

        if (phy.m_collisionShape == CollisionShape::Sphere)
        {
            PxSphereGeometry geo;
            shape->getSphereGeometry(geo);
            geo.radius = phy.m_radius * data.GetScale().Avg();
            shape->setGeometry(geo);
        }
        else if (phy.m_collisionShape == CollisionShape::Box)
        {
            PxBoxGeometry geo;
            shape->getBoxGeometry(geo);
            geo.halfExtents = ToPxVector3(phy.m_halfExtents * data.GetScale());
            shape->setGeometry(geo);
        }
        else if (phy.m_collisionShape == CollisionShape::Capsule)
        {
            PxCapsuleGeometry geo;
            shape->getCapsuleGeometry(geo);
            geo.radius = phy.m_radius * data.GetScale().Avg();
            ;
            geo.halfHeight = phy.m_capsuleHalfHeight * data.GetScale().y;
            shape->setGeometry(geo);
        }
        else if (phy.m_collisionShape == CollisionShape::ConvexMesh)
        {
            PxConvexMeshGeometry geo;
            shape->getConvexMeshGeometry(geo);
            geo.scale.scale = ToPxVector3(data.GetScale());
            shape->setGeometry(geo);
        }
    }

    void PhysicsEngine::RecreateBodyShape(ECS::Entity body)
    {
        auto& phy = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);

        if (phy.m_simType != SimulationType::None)
        {
            PxU32    nbShapes     = 0;
            PxShape* currentShape = nullptr;
            PxShape* newShape     = GetCreateShape(phy);
            auto*    actor        = m_actors[body];

            if (phy.m_simType == SimulationType::Dynamic)
            {
                currentShape = m_shapes[body];
                actor->detachShape(*currentShape);
                actor->attachShape(*newShape);
            }
            else
            {
                currentShape = m_shapes[body];
                actor->detachShape(*currentShape);
                actor->attachShape(*newShape);
            }

            m_shapes[body] = newShape;
            newShape->release();
        }
    }

    void PhysicsEngine::SetBodyKinematic(ECS::Entity body, bool kinematic)
    {
        auto& phy         = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        phy.m_isKinematic = kinematic;

        if (phy.m_simType == SimulationType::Dynamic)
        {
            auto* act = (PxRigidDynamic*)m_actors[body];
            act->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);

            if (!kinematic)
                act->wakeUp();
        }
    }

    void PhysicsEngine::OnLevelInstalled(const Event::ELevelInstalled& ev)
    {
        ECS::Registry::Get()->on_destroy<ECS::PhysicsComponent>().connect<&PhysicsEngine::OnPhysicsComponentRemoved>(this);
        ECS::Registry::Get()->on_construct<ECS::PhysicsComponent>().connect<&PhysicsEngine::OnPhysicsComponentAdded>(this);
        ECS::Registry::Get()->on_construct<ECS::EntityDataComponent>().connect<&PhysicsEngine::OnPhysicsComponentAdded>(this);

        m_defaultMaterial = Resources::ResourceStorage::Get()->GetResource<PhysicsMaterial>("Resources/Engine/Physics/Materials/DefaultPhysicsMaterial.linaphymat");
        m_pxDefaultMaterial->setStaticFriction(m_defaultMaterial->m_staticFriction);
        m_pxDefaultMaterial->setDynamicFriction(m_defaultMaterial->m_dynamicFriction);
        m_pxDefaultMaterial->setRestitution(m_defaultMaterial->m_restitution);

        auto view = ECS::Registry::Get()->view<ECS::PhysicsComponent>();

        for (auto entity : view)
        {
            ECS::PhysicsComponent& phyComp = view.get<ECS::PhysicsComponent>(entity);

            if (phyComp.m_simType == SimulationType::Dynamic)
                AddBodyToWorld(entity, true);
            else if (phyComp.m_simType == SimulationType::Static)
                AddBodyToWorld(entity, false);
        }
    }

    void PhysicsEngine::OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent)
    {
        RemoveBodyFromWorld(ent);
    }

    void PhysicsEngine::OnEntityEnabledChanged(const Event::EEntityEnabledChanged& ev)
    {

        if (IsEntityAPhysicsActor(ev.m_entity))
        {
            if (!ev.m_enabled)
                RemoveBodyFromWorld(ev.m_entity);
        }
        else
        {
            auto& phy = ECS::Registry::Get()->get<ECS::PhysicsComponent>(ev.m_entity);

            if (ev.m_enabled && phy.m_simType != SimulationType::None)
                AddBodyToWorld(ev.m_entity, phy.m_simType == SimulationType::Dynamic);
        }
    }

    void PhysicsEngine::RemoveBodyFromWorld(ECS::Entity body)
    {
        if (m_actors.find(body) == m_actors.end())
            return;
        LINA_TRACE("Removing body from the world. {0}", body);
        m_actors[body]->release();
        m_actors.erase(body);
    }

    void PhysicsEngine::AddBodyToWorld(ECS::Entity body, bool isDynamic)
    {
        if (isDynamic && m_actors.find(body) != m_actors.end())
            return;

        ECS::EntityDataComponent& data    = ECS::Registry::Get()->get<ECS::EntityDataComponent>(body);
        ECS::PhysicsComponent&    phyComp = ECS::Registry::Get()->get<ECS::PhysicsComponent>(body);
        PxTransform               pose;
        pose.p         = ToPxVector3(data.GetLocation());
        pose.q         = ToPxQuat(data.GetRotation());
        PxShape* shape = GetCreateShape(phyComp);

        if (isDynamic)
        {
            LINA_TRACE("Adding a dynamic actor to the world. {0}", body);
            PxRigidDynamic* rigid = m_pxPhysics->createRigidDynamic(pose);
            rigid->attachShape(*shape);
            physx::PxRigidBodyExt::updateMassAndInertia(*rigid, 10.0f);
            rigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, phyComp.GetIsKinematic());
            m_actors[body] = rigid;
            m_pxScene->addActor(*rigid);
            m_shapes[body] = shape;
        }
        else
        {
            LINA_TRACE("Adding a static actor to the world. {0}", body);
            PxRigidStatic* stc = m_pxPhysics->createRigidStatic(pose);
            stc->attachShape(*shape);
            m_actors[body] = stc;
            m_pxScene->addActor(*stc);
            m_shapes[body] = shape;
        }

        shape->release();
    }

    physx::PxActor** PhysicsEngine::GetActiveActors(uint32& size)
    {
        return m_pxScene->getActiveActors(size);
    }

    ECS::Entity PhysicsEngine::GetEntityOfActor(physx::PxActor* actor)
    {
        for (auto& p : m_actors)
        {
            if (p.second == actor)
                return p.first;
        }

        return entt::null;
    }

    std::map<ECS::Entity, physx::PxRigidActor*>& PhysicsEngine::GetAllActors()
    {
        return m_actors;
    }

    std::map<StringIDType, physx::PxMaterial*>& PhysicsEngine::GetMaterials()
    {
        return m_materials;
    }

} // namespace Lina::Physics
