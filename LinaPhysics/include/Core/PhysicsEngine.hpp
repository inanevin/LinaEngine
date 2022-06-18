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

/*
Class: PhysicsEngine

Responsible for initializing, running and cleaning up the physics world. Also a wrapper for bt3.

Timestamp: 5/1/2019 2:35:28 AM
*/

#pragma once

#ifndef PhysicsEngine_HPP
#define PhysicsEngine_HPP

#include "Core/PhysicsCooker.hpp"
#include "Core/CommonECS.hpp"
#include "ECS/Components/PhysicsComponent.hpp"
#include "ECS/SystemList.hpp"
#include "ECS/Systems/RigidbodySystem.hpp"
#include "Physics/PhysicsMaterial.hpp"

namespace Lina
{
    class Engine;

    namespace ECS
    {
        struct PhysicsComponent;
    }
    namespace Event
    {
        class EventSystem;
        struct ELevelInstalled;
        struct EPostSceneDraw;
        struct ELoadResourceFromFile;
        struct ELoadResourceFromMemory;
        struct ELoadResourceFromMemory;
        struct EEntityEnabledChanged;
    } // namespace Event
} // namespace Lina

namespace physx
{
    class PxShape;
    class PxRigidActor;
    class PxActor;
    class PxMaterial;
} // namespace physx

namespace Lina::Physics
{

    class PhysicsEngine
    {
    public:
        static PhysicsEngine* Get()
        {
            return s_physicsEngine;
        }

        /// <summary>
        /// Given a convex mesh buffer data, creates a convex mesh object and stores it.
        /// </summary>
        void CreateConvexMesh(std::vector<uint8>& data, StringIDType sid, int nodeID);

        /// <summary>
        /// Returns all moving actor within the Nvidia PhysX scene.
        /// </summary>
        physx::PxActor** GetActiveActors(uint32& size);

        /// <summary>
        /// Given a PxActor, returns the Entity it belongs to.
        /// </summary>
        ECS::Entity GetEntityOfActor(physx::PxActor* actor);

        /// <summary>
        /// Returns all Dynamic bodies, regardless of whether they are static or not.
        /// </summary>
        std::map<ECS::Entity, physx::PxRigidActor*>& GetAllActors();

        /// <summary>
        /// Returns a map of created physics material, the key represents the Lina ID of the PhysicsMaterial object,
        /// value is the PhysX object.
        /// </summary>
        std::map<StringIDType, physx::PxMaterial*>& GetMaterials();

        /// <summary>
        /// Returns true if the given entity is alive in the physics world, static or dynamic.
        /// </summary>
        bool IsEntityAPhysicsActor(ECS::Entity ent);

        /// <summary>
        /// Gets a const reference to the main physics pipeline, which contains a list of physics systems.
        /// </summary>
        inline const ECS::SystemList& GetPipeline()
        {
            return m_physicsPipeline;
        }

        /// <summary>
        /// Any system added to the physics pipeline will get updated during the physics update.
        /// </summary>
        void AddToPhysicsPipeline(ECS::System& system);

        inline PhysicsMaterial* GetDefaultPhysicsMaterial()
        {
            return m_defaultMaterial;
        }

        void SetMaterialStaticFriction(PhysicsMaterial& mat, float friction);
        void SetMaterialDynamicFriction(PhysicsMaterial& mat, float friction);
        void SetMaterialRestitution(PhysicsMaterial& mat, float restitution);
        void SetBodySimulation(ECS::Entity body, SimulationType type);
        void SetBodyCollisionShape(ECS::Entity body, Physics::CollisionShape shape);
        void SetBodyMass(ECS::Entity body, float mass);
        void SetBodyMaterial(ECS::Entity body, PhysicsMaterial* mat);
        void SetBodyRadius(ECS::Entity body, float radius);
        void SetBodyHeight(ECS::Entity body, float height);
        void SetBodyHalfExtents(ECS::Entity body, const Vector3& extents);
        void SetBodyKinematic(ECS::Entity body, bool kinematic);
        void UpdateBodyShapeParameters(ECS::Entity body);

        void SetDebugDraw(bool enabled)
        {
            m_debugDrawEnabled = enabled;
        }

    private:
        friend class Engine;
        friend struct ECS::PhysicsComponent;

        PhysicsEngine();
        ~PhysicsEngine();
        void  Initialize(ApplicationMode appMode);
        void  Tick(float fixedDelta);
        void  Shutdown();
        float GetStepTime()
        {
            return m_stepTime;
        }

    private:
        void            OnPhysicsComponentAdded(entt::registry& reg, entt::entity ent);
        void            RecreateBodyShape(ECS::Entity body);
        void            OnLevelInstalled(const Event::ELevelInstalled& ev);
        void            OnPostSceneDraw(const Event::EPostSceneDraw&);
        void            OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent);
        void            OnEntityEnabledChanged(const Event::EEntityEnabledChanged& ev);
        void            RemoveBodyFromWorld(ECS::Entity body);
        void            AddBodyToWorld(ECS::Entity body, bool isDynamic);
        physx::PxShape* GetCreateShape(ECS::PhysicsComponent& phy, ECS::Entity ent = entt::null);

    private:
        static PhysicsEngine* s_physicsEngine;
        ECS::RigidbodySystem       m_rigidbodySystem;
        ECS::SystemList            m_physicsPipeline;
        Event::EventSystem*        m_eventSystem;
        ApplicationMode            m_appMode = ApplicationMode::Editor;
        PhysXCooker                m_cooker;
        PhysicsMaterial*           m_defaultMaterial  = nullptr;
        bool                       m_debugDrawEnabled = false;
        float                      m_stepTime         = 0.016f;
    };
} // namespace Lina::Physics

#endif
