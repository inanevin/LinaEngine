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

#include "Core/Backend/OpenGL/RenderEngineOpenGL.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/Log.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/ResourceManager.hpp"

namespace Lina::Graphics
{
	RenderEngineOpenGL::~RenderEngineOpenGL()
	{
		if (m_initialized)
			DisconnectEvents();
	}

	void RenderEngineOpenGL::DisconnectEvents()
	{
		m_eventSys->Disconnect<Event::EPreMainLoop>(this);
		m_eventSys->Disconnect<Event::EPostMainLoop>(this);
		m_eventSys->Disconnect<Event::ERender>(this);
		m_eventSys->Disconnect<Event::EAppLoad>(this);
		m_eventSys->Disconnect<Event::ETick>(this);
		m_eventSys->Disconnect<Event::EWindowResized>(this);
	}

	void RenderEngineOpenGL::SetReferences(Event::EventSystem* eventSys, ECS::Registry* ecs, Resources::ResourceManager* resources)
	{
		m_eventSys = eventSys;
		m_ecs = ecs;
		m_resources = resources;
		m_eventSys->Connect<Event::EPreMainLoop, &RenderEngineOpenGL::OnPreMainLoop>(this);
		m_eventSys->Connect<Event::EPostMainLoop, &RenderEngineOpenGL::OnPostMainLoop>(this);
		m_eventSys->Connect<Event::ERender, &RenderEngineOpenGL::Render>(this);
		m_eventSys->Connect<Event::EAppLoad, &RenderEngineOpenGL::OnAppLoad>(this);
		m_eventSys->Connect<Event::ETick, &RenderEngineOpenGL::Tick>(this);
		m_eventSys->Connect<Event::EWindowResized, &RenderEngineOpenGL::OnWindowResize>(this);
	}

	void RenderEngineOpenGL::OnAppLoad(Event::EAppLoad& e)
	{
		// First create empty window context.
		m_window.SetReferences(m_eventSys, e.m_appInfo->m_windowProperties);
		m_window.CreateContext();

		if (!m_initialized)
			DisconnectEvents();
	}

	void RenderEngineOpenGL::OnPreMainLoop(Event::EPreMainLoop& e)
	{
		LINA_TRACE("[Render Engine Vulkan] -> Startup");

	}

	void RenderEngineOpenGL::OnPostMainLoop(Event::EPostMainLoop& e)
	{
		LINA_TRACE("[Render Engine Vulkan] -> Shutdown");

	}


	void RenderEngineOpenGL::OnWindowResize(Event::EWindowResized& e)
	{
		
	}

	void RenderEngineOpenGL::Tick()
	{
		PROFILER_FUNC();

	}

	void RenderEngineOpenGL::Render()
	{
		PROFILER_FUNC();

		m_eventSys->Trigger<Event::EPreRender>();
		m_eventSys->Trigger<Event::EPostRender>();
		m_eventSys->Trigger<Event::EFinalizePostRender>();
	}
}