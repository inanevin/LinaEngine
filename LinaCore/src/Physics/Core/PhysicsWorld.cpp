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

#include "Physics/Core/PhysicsWorld.hpp"
#include "System/ISystem.hpp"
#include "Core/SystemInfo.hpp"
#include "World/Core/EntityWorld.hpp"

namespace Lina
{

	void PhysicsWorld::Tick(float delta)
	{
		static float physicsAccumulator = 0.0f;
		const float	 phydt				= SystemInfo::GetPhysicsDeltaTime();
		m_simulated						= false;

		if (physicsAccumulator > phydt)
		{
			// N = accumulator - update rate -> remainder.
			// TODO: optional substepping to compansate.
			physicsAccumulator = 0;

			// dispatch events.
			Event eventData;
			eventData.fParams[0] = phydt;
			m_world->DispatchEvent(EVG_Physics, eventData);
			m_world->DispatchEvent(EVG_PostPhysics, eventData);

			// Simulate nvidia physx
			m_simulated = true;
		}

		physicsAccumulator += delta;
	}

	void PhysicsWorld::WaitForSimulation()
	{
		if (m_simulated)
		{
			// fetch physx results
		}
	}

	void PhysicsWorld::SyncData(float alpha)
	{
	}
} // namespace Lina
