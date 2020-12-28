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
SOFTWARE.*/

#include "Core/Backend/Vulkan/RenderEngineVulkan.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/Log.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/Backend/Vulkan/Utility/VulkanFunctions.hpp"
#include "Core/Backend/Vulkan/Objects/VulkanFence.hpp"
#include "Core/Backend/Vulkan/Objects/VulkanSemaphore.hpp"


namespace Lina::Graphics
{

	RenderEngineVulkan::~RenderEngineVulkan()
	{
		if (m_initialized)
		{
			m_eventSys->Disconnect<Event::EPreMainLoop>(this);
			m_eventSys->Disconnect<Event::EPostMainLoop>(this);
			m_eventSys->Disconnect<Event::ERender>(this);
			m_eventSys->Disconnect<Event::EEarlyInit>(this);
		}
	}

	void RenderEngineVulkan::SetReferences(Event::EventSystem* eventSys, ECS::Registry* ecs)
	{
		m_eventSys = eventSys;
		m_ecs = ecs;
		m_eventSys->Connect<Event::EPreMainLoop, &RenderEngineVulkan::OnPreMainLoop>(this);
		m_eventSys->Connect<Event::EPostMainLoop, &RenderEngineVulkan::OnPostMainLoop>(this);
		m_eventSys->Connect<Event::ERender, &RenderEngineVulkan::Render>(this);
		m_eventSys->Connect<Event::EEarlyInit, &RenderEngineVulkan::OnEarlyInit>(this);
	}
	void RenderEngineVulkan::OnEarlyInit(Event::EEarlyInit& e)
	{
		// First create empty window context.
		m_window.SetReferences(m_eventSys, e.m_appInfo->m_windowProperties);
		m_window.CreateContext();

		// Create vulkan instance
		if (m_loader.InitializeVulkan(e.m_appInfo, &m_vulkanData))
		{
			// Create surface & logical device.
			m_window.CreateWindowSurface(m_vulkanData.m_instance, m_vulkanData.m_surface);
			m_loader.CreateLogicalDevice();
			m_logicalDevice.m_handle = m_vulkanData.m_logicalDevice;

			// Create a swapchain.
			SwapchainData data;
			data.vulkanData = &m_vulkanData;
			data.windowProps = &e.m_appInfo->m_windowProperties;
			m_swapchain.Create(data);

			VkImage img = m_swapchain.GetImage(m_vulkanData.m_logicalDevice, VK_NULL_HANDLE);

			VkCommandPool pool = m_logicalDevice.CommandPoolCreate(m_vulkanData.m_graphicsQueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
			std::vector<VkCommandBuffer> primaryBuffers = m_logicalDevice.CommandBufferCreate(pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
			m_logicalDevice.CommandBufferCreate(pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1);
			m_logicalDevice.CommandBufferBeginPrimary(0, VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT);

			
			SecondaryCommandBufferData secdata;
			m_logicalDevice.CommandBufferBeginSecondary(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, secdata);

			m_logicalDevice.CommandBufferEnd(0);
			m_logicalDevice.CommandBufferEnd(0);
			m_secondaryBuffer.Reset(0, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
			m_pool.Reset(m_vulkanData.m_logicalDevice, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

			std::vector<VkFence> fences;
			VkBool32 waitForAll = VK_TRUE;
			uint64_t timeout = 2000000000;
			// We can pass a 0 timeout and check the VkResult to see if the fence is available or not.
			vkWaitForFences(m_vulkanData.m_logicalDevice, static_cast<uint32_t>(fences.size()), &fences[0], waitForAll, timeout);

			m_initialized = true;
		}

		if (!m_initialized)
		{
			m_eventSys->Disconnect<Event::EPreMainLoop>(this);
			m_eventSys->Disconnect<Event::EPostMainLoop>(this);
			m_eventSys->Disconnect<Event::ERender>(this);
			m_eventSys->Disconnect<Event::EEarlyInit>(this);
		}
	}

	void RenderEngineVulkan::OnPreMainLoop(Event::EPreMainLoop& e)
	{
		LINA_TRACE("[Render Engine Vulkan] -> Startup");
	}

	void RenderEngineVulkan::OnPostMainLoop(Event::EPostMainLoop& e)
	{
		LINA_TRACE("[Render Engine Vulkan] -> Shutdown");
		m_swapchain.Destroy(m_vulkanData.m_logicalDevice);

		if (m_vulkanData.m_surface)
		{
			vkDestroySurfaceKHR(m_vulkanData.m_instance, m_vulkanData.m_surface, nullptr);
			m_vulkanData.m_surface = VK_NULL_HANDLE;
		}

		m_loader.Unload();
		m_window.Terminate();
	}

	void RenderEngineVulkan::Tick()
	{
		PROFILER_FUNC();
	}

	void RenderEngineVulkan::Render()
	{
		PROFILER_FUNC();

		m_eventSys->Trigger<Event::EPreRender>();
		m_eventSys->Trigger<Event::EPostRender>();
		m_eventSys->Trigger<Event::EFinalizePostRender>();
	}
}