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

#include "Core/Engine.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Log/Log.hpp"

namespace Lina
{
	void Engine::Initialize(const ApplicationInfo& appInfo)
	{
		LINA_TRACE("[Initialize] -> Core Engine {0}", typeid(*this).name());
		//Event::EventSystem::s_eventSystem = &m_eventSystem;
		//ECS::Registry::s_ecs = &m_ecs;
		//Graphics::WindowBackend::s_openglWindo w = &m_window;
		//Graphics::RenderEngineBackend::s_renderEngine = &m_renderEngine;
		//Physics::PhysicsEngineBackend::s_physicsEngine = &m_physicsEngine;
		//Input::InputEngineBackend::s_inputEngine = &m_inputEngine;

		m_running = true;
		//m_eventSystem.Trigger<Event::EInitializeEngine>(Event::EInitializeEngine{ appInfo });
		Run();
	}

	void Engine::Run()
	{
		//m_eventSystem.Trigger<Event::EStartGame>(Event::EStartGame{ });

		while (m_running)
		{

		}

		//m_eventSystem.Trigger<Event::EEndGame>(Event::EEndGame{ });
		Shutdown();
	}

	void Engine::Shutdown()
	{
		LINA_TRACE("[Shutdown] -> Core Engine {0}", typeid(*this).name());
		//m_eventSystem.Trigger<Event::EShutdownEngine>(Event::EShutdownEngine{});
	}
}