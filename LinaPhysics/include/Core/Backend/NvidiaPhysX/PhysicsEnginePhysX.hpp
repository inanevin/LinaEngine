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

/*
Class: PhysicsEnginePhysX

Responsible for initializing, running and cleaning up the physics world. Also a wrapper for bt3.

Timestamp: 5/1/2019 2:35:28 AM
*/

#pragma once

#ifndef PhysicsEnginePhysx_HPP
#define PhysicsEnginePhysx_HPP

#include "ECS/ECS.hpp"
#include "EventSystem/Events.hpp"
#include <linaphysics_export.h>
#include "JobSystem/JobSystem.hpp"
namespace Lina
{
	namespace Event
	{
		class EventSystem;
	}
}

namespace Lina::Physics
{

	class PhysicsEnginePhysX 
	{
	public:


	private:

		friend class Application;

		PhysicsEnginePhysX& operator=(const PhysicsEnginePhysX&) = delete;
		PhysicsEnginePhysX(const PhysicsEnginePhysX&) = delete;
		PhysicsEnginePhysX() {};
		~PhysicsEnginePhysX();

		void SetReferences(Event::EventSystem* eventSys, ECS::Registry* ecs);

	private:

		void OnPreMainLoop(Event::EPreMainLoop& e);
		void OnPostMainLoop(Event::EPostMainLoop& e);
		void StartTick();
		void OnPoll();

	private:
		Executor m_executor;
		TaskFlow m_taskflow;
		Event::EventSystem* m_eventSys = nullptr;
		ECS::Registry* m_ecs = nullptr;
		bool m_isRunning = false;

	};
}


#endif