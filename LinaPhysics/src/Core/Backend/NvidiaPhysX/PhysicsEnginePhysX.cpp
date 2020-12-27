/* 
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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

#include "Core/Backend/NvidiaPhysX/PhysicsEnginePhysX.hpp"  
#include "Profiling/Profiler.hpp"
#include "Utility/EngineUtility.hpp"
#include "Core/Log.hpp"
#include "JobSystem/JobSystem.hpp"
#include "EventSystem/EventSystem.hpp"
#include "ECS/ECS.hpp"

#define FIXED_TIMESTEP 0.01

namespace Lina::Physics
{
	Executor m_executor;
	TaskFlow m_taskflow;

	PhysicsEnginePhysX::~PhysicsEnginePhysX()
	{
		m_eventSys->Disconnect<Event::EPreMainLoop>(this);
		m_eventSys->Disconnect<Event::EPostMainLoop>(this);
	}
	
	void PhysicsEnginePhysX::SetReferences(Event::EventSystem* eventSys, ECS::Registry* ecs)
	{
		m_eventSys = eventSys;
		m_ecs = ecs;
		m_eventSys->Connect<Event::EPreMainLoop, &PhysicsEnginePhysX::OnPreMainLoop>(this);
		m_eventSys->Connect<Event::EPostMainLoop, &PhysicsEnginePhysX::OnPostMainLoop>(this);
	}

	void PhysicsEnginePhysX::OnPreMainLoop(Event::EPreMainLoop& e)
	{
		LINA_TRACE("[Physics Engine Nvidia PhysX] -> Startup");
		m_taskflow.emplace([this]() {OnPoll(); });
		m_executor.run(m_taskflow);
	}

	void PhysicsEnginePhysX::OnPostMainLoop(Event::EPostMainLoop& e)
	{
		m_isRunning = false;
		LINA_TRACE("[Physics Engine Nvidia PhysX] -> Shutdown");
	}

	void PhysicsEnginePhysX::OnPoll()
	{
		PROFILER_THREAD_SCOPE("Physics");
		PROFILER_FUNC();

		m_isRunning = true;
		
		double t = 0.0;

		double currentTime = Utility::GetCPUTime();
		double accumulator = 0.0;

		while (m_isRunning)
		{

			double newTime = Utility::GetCPUTime();
			double frameTime = newTime - currentTime;

			if (frameTime > 0.25)
				frameTime = 0.25;
			currentTime = newTime;
			accumulator += frameTime;

			while (accumulator >= FIXED_TIMESTEP)
			{
				// Calculate physics using t & dt.
				m_eventSys->Trigger<Event::EPrePhysicsTick>({FIXED_TIMESTEP, true});
				m_eventSys->Trigger<Event::EPhysicsTick>({FIXED_TIMESTEP, true});
				m_eventSys->Trigger<Event::EPostPhysicsTick>({FIXED_TIMESTEP, true});
				
				t += FIXED_TIMESTEP;
				accumulator -= FIXED_TIMESTEP;
			}
		}
	}
}

