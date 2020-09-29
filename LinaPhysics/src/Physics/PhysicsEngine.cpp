/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: PhysicsEngine
Timestamp: 5/1/2019 2:35:43 AM

*/

#include "Physics/PhysicsEngine.hpp"  
#include "Utility/Log.hpp"
#include "btBulletDynamicsCommon.h"

namespace LinaEngine::Physics
{
	PhysicsEngine::PhysicsEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	PhysicsEngine::~PhysicsEngine()
	{
		LINA_CORE_TRACE("[Destructor] -> Physics Engine ({0})", typeid(*this).name());
	}

	void PhysicsEngine::Initialize()
	{
		LINA_CORE_TRACE("[Initialization] -> Physics Engine ({0})", typeid(*this).name());

		///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		// Create dynamics world
		btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -10, 0));
	
		
	}

	void PhysicsEngine::Tick(float fixedDelta)
	{
		
	}

	void PhysicsEngine::CleanUp()
	{

	}
}

