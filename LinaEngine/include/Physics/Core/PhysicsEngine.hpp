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

#pragma once

#ifndef PhysicsEngine_HPP
#define PhysicsEngine_HPP

#include "Physics/Core/PhysicsCooker.hpp"
#include "Physics/Components/PhysicsComponent.hpp"
#include "Physics/Core/RigidbodySystem.hpp"
#include "Physics/Core/PhysicsMaterial.hpp"

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
        struct ELoadResourceFromFile;
        struct ELoadResourceFromMemory;
        struct ELoadResourceFromMemory;
    } // namespace Event

    namespace World
    {
        class Entity;
    }
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

        /// <summary>
        /// Given a convex mesh buffer data, creates a convex mesh object and stores it.
        /// </summary>
        void CreateConvexMesh(Vector<uint8>& data, StringID sid, int nodeID);

        /// <summary>
        /// Returns all moving actor within the Nvidia PhysX scene.
        /// </summary>
        physx::PxActor** GetActiveActors(uint32& size);

        /// <summary>
        /// Given a PxActor, returns the Entity it belongs to.
        /// </summary>
        World::Entity* GetEntityOfActor(physx::PxActor* actor);

        /// <summary>
        /// Returns all Dynamic bodies, regardless of whether they are static or not.
        /// </summary>
        HashMap<World::Entity*, physx::PxRigidActor*>& GetAllActors();

        /// <summary>
        /// Returns a map of created physics material, the key represents the Lina ID of the PhysicsMaterial object,
        /// value is the PhysX object.
        /// </summary>
        HashMap<StringID, physx::PxMaterial*>& GetMaterials();

        /// <summary>
        /// Returns true if the given entity is alive in the physics world, static or dynamic.
        /// </summary>
        bool IsEntityAPhysicsActor(World::Entity* ent);

        inline PhysicsMaterial* GetDefaultPhysicsMaterial()
        {
            return m_defaultMaterial;
        }

        void SetMaterialStaticFriction(PhysicsMaterial& mat, float friction);
        void SetMaterialDynamicFriction(PhysicsMaterial& mat, float friction);
        void SetMaterialRestitution(PhysicsMaterial& mat, float restitution);
        void SetBodySimulation(World::Entity* body, SimulationType type);
        void SetBodyCollisionShape(World::Entity* body, Physics::CollisionShape shape);
        void SetBodyMass(World::Entity* body, float mass);
        void SetBodyMaterial(World::Entity* body, PhysicsMaterial* mat);
        void SetBodyRadius(World::Entity* body, float radius);
        void SetBodyHeight(World::Entity* body, float height);
        void SetBodyHalfExtents(World::Entity* body, const Vector3& extents);
        void SetBodyKinematic(World::Entity* body, bool kinematic);
        void UpdateBodyShapeParameters(World::Entity* body);

        void SetDebugDraw(bool enabled)
        {
            m_debugDrawEnabled = enabled;
        }

    private:
        friend class Engine;
        friend struct PysicsComponent;

        PhysicsEngine()  = default;
        ~PhysicsEngine() = default;

        void  Initialize();
        void  Tick(float fixedDelta);
        void  Shutdown();
        float GetStepTime()
        {
            return m_stepTime;
        }

    private:
        // void            OnPhysicsComponentAdded(entt::registry& reg, entt::entity ent);
        void RecreateBodyShape(World::Entity* body);
        void OnLevelInstalled(const Event::ELevelInstalled& ev);
        // void OnPostSceneDraw(const Event::EPostSceneDraw&);
        // void            OnPhysicsComponentRemoved(entt::registry& reg, entt::entity ent);
        // void            OnEntityEnabledChanged(const Event::EEntityEnabledChanged& ev);
        void            RemoveBodyFromWorld(World::Entity* body);
        void            AddBodyToWorld(World::Entity* body, bool isDynamic);
        physx::PxShape* GetCreateShape(PhysicsComponent& phy, World::Entity* ent = nullptr);

    private:
        RigidbodySystem       m_rigidbodySystem;
        Event::EventSystem*   m_eventSystem;
        PhysXCooker           m_cooker;
        PhysicsMaterial*      m_defaultMaterial  = nullptr;
        bool                  m_debugDrawEnabled = false;
        float                 m_stepTime         = 0.016f;
    };
} // namespace Lina::Physics

#endif
