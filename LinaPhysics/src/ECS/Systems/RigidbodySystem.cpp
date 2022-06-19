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

#include "ECS/Systems/RigidbodySystem.hpp"

#include "Core/PhysicsEngine.hpp"
#include "Core/PhysicsCommon.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/PhysicsComponent.hpp"
#include "ECS/Registry.hpp"

#ifdef LINA_PHYSICS_PHYSX
#include "PxPhysics.h"
#endif

using namespace physx;
namespace Lina::ECS
{

    void RigidbodySystem::Initialize(const String& name, Physics::PhysicsEngine* engine)
    {
        System::Initialize(name);
        m_engine = engine;
    }

    void RigidbodySystem::UpdateComponents(float delta)
    {
        auto* physicsEngine = Physics::PhysicsEngine::Get();
        auto* ecs               = ECS::Registry::Get();


#ifdef LINA_PHYSICS_BULLET

        auto view = ecs->view<EntityDataComponent, PhysicsComponent>();

        // Find all entities with rigidbody component and transform component attached to them.
        for (auto entity : view)
        {
            PhysicsComponent& phyComp = view.get<PhysicsComponent>(entity);
            if (!phyComp.GetIsSimulated())
                continue;
            EntityDataComponent& data = view.get<EntityDataComponent>(entity);
            btRigidBody*         rb   = Physics::PhysicsEngine::Get()->GetActiveRigidbody(entity);
            btTransform          btTrans;
            rb->getMotionState()->getWorldTransform(btTrans);
            Vector3 location = Physics::ToLinaVector(rb->getWorldTransform().getOrigin());
            data.SetLocation(location);
            data.SetRotation(Quaternion(btTrans.getRotation().getX(), btTrans.getRotation().getY(), btTrans.getRotation().getZ(), btTrans.getRotation().getW()));
            phyComp.m_angularVelocity = Physics::ToLinaVector(rb->getAngularVelocity());
            phyComp.m_velocity        = Physics::ToLinaVector(rb->getLinearVelocity());
            phyComp.m_turnVelocity    = Physics::ToLinaVector(rb->getTurnVelocity());
        }
#endif
#ifdef LINA_PHYSICS_PHYSX

        auto& actors = physicsEngine->GetAllActors();
        m_poolSize   = (int)actors.size();

        for (auto& p : actors)
        {
            EntityDataComponent& data    = ecs->get<EntityDataComponent>(p.first);
            PhysicsComponent&    phyComp = ecs->get<PhysicsComponent>(p.first);
            if (phyComp.m_simType == Physics::SimulationType::Static)
                continue;

            if (phyComp.m_isKinematic)
            {
                PxTransform destination;
                destination.p = Physics::ToPxVector3(data.GetLocation());
                destination.q = Physics::ToPxQuat(data.GetRotation());
                ((PxRigidDynamic*)p.second)->setKinematicTarget(destination);
                m_engine->UpdateBodyShapeParameters(p.first);
            }
            else
            {
                PxU32     nbActiveActors;
                PxActor** activeActors = m_engine->GetActiveActors(nbActiveActors);
                for (PxU32 i = 0; i < nbActiveActors; ++i)
                {
                    ECS::Entity entity = m_engine->GetEntityOfActor(activeActors[i]);

                    if (entity == p.first)
                    {
                        PxRigidDynamic* rigid = static_cast<PxRigidDynamic*>(activeActors[i]);
                        auto&           pose  = rigid->getGlobalPose();
                        data.SetLocation(Physics::ToLinaVector3(pose.p));
                        data.SetRotation(Physics::ToLinaQuat(pose.q));
                    }
                }
            }
        }

#endif
    }
} // namespace Lina::ECS
