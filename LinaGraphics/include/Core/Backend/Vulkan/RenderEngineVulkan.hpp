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
Class: RenderEngineVulkan

Responsible for managing the whole rendering pipeline, creating frame buffers, textures,
materials, meshes etc. Also handles most of the memory management for the rendering pipeline.

Timestamp: 4/15/2019 12:26:31 PM
*/

#pragma once

#ifndef RenderEngineVulkan_HPP
#define RenderEngineVulkan_HPP

#include "Core/Common.hpp"
#include "ECS/ECS.hpp"
#include "EventSystem/Events.hpp"
#include "VulkanLoader.hpp"
#include "WindowVulkan.hpp"
#include "VulkanSwapchain.hpp"

namespace Lina
{
	namespace Event
	{
		class EventSystem;
	}
}

namespace Lina::Graphics
{
	class RenderEngineVulkan
	{
	public:


	private:

		friend class Application;

		RenderEngineVulkan& operator=(const RenderEngineVulkan&) = delete;
		RenderEngineVulkan(const RenderEngineVulkan&) = delete;
		RenderEngineVulkan() {};
		~RenderEngineVulkan();

		void SetReferences(Event::EventSystem* eventSys, ECS::Registry* ecs);
		inline WindowVulkan* GetWindow() { return &m_window; }

	private:

		void OnEarlyInit(Event::EEarlyInit& e);
		void OnPreMainLoop(Event::EPreMainLoop& e);
		void OnPostMainLoop(Event::EPostMainLoop& e);
		void Tick();
		void Render();

	private:

		bool m_initialized = false;
		VulkanData m_vulkanData;
		VulkanLoader m_loader;
		VulkanSwapchain m_swapchain;
		Event::EventSystem* m_eventSys = nullptr;
		ECS::Registry* m_ecs = nullptr;
		WindowVulkan m_window;
	};

}


#endif