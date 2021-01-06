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
#include "Core/ResourceManager.hpp"

namespace Lina::Graphics
{
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	VkPipelineLayout layout;
	VkRenderPass rp;
	std::vector<VkCommandBuffer> commandBuffers;
	VkCommandPool pool;
	VkPipeline pipeline;
	std::vector<VkFramebuffer> framebuffers;
	const int MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	bool m_windowResized = false;

	RenderEngineVulkan::~RenderEngineVulkan()
	{
		if (m_initialized)
			DisconnectEvents();
	}

	void RenderEngineVulkan::DisconnectEvents()
	{
		m_eventSys->Disconnect<Event::EPreMainLoop>(this);
		m_eventSys->Disconnect<Event::EPostMainLoop>(this);
		m_eventSys->Disconnect<Event::ERender>(this);
		m_eventSys->Disconnect<Event::EAppLoad>(this);
		m_eventSys->Disconnect<Event::ETick>(this);
		m_eventSys->Disconnect<Event::EWindowResized>(this);
	}

	void RenderEngineVulkan::SetReferences(Event::EventSystem* eventSys, ECS::Registry* ecs, Resources::ResourceManager* resources)
	{
		m_eventSys = eventSys;
		m_ecs = ecs;
		m_resources = resources;
		m_eventSys->Connect<Event::EPreMainLoop, &RenderEngineVulkan::OnPreMainLoop>(this);
		m_eventSys->Connect<Event::EPostMainLoop, &RenderEngineVulkan::OnPostMainLoop>(this);
		m_eventSys->Connect<Event::ERender, &RenderEngineVulkan::Render>(this);
		m_eventSys->Connect<Event::EAppLoad, &RenderEngineVulkan::OnAppLoad>(this);
		m_eventSys->Connect<Event::ETick, &RenderEngineVulkan::Tick>(this);
		m_eventSys->Connect<Event::EWindowResized, &RenderEngineVulkan::OnWindowResize>(this);
	}

	void RenderEngineVulkan::OnAppLoad(Event::EAppLoad& e)
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
			m_logicalDevice.m_physicalDevice = m_vulkanData.m_physicalDevice;

			// Create a swapchain.
			SwapchainData data;
			data.vulkanData = &m_vulkanData;
			data.windowProps = &e.m_appInfo->m_windowProperties;
			m_swapchain.Create(data);

			m_initialized = true;
		}

		if (!m_initialized)
			DisconnectEvents();
	}

	void RenderEngineVulkan::OnPreMainLoop(Event::EPreMainLoop& e)
	{
		LINA_TRACE("[Render Engine Vulkan] -> Startup");

		pool = m_logicalDevice.CommandPoolCreate(m_vulkanData.m_graphicsQueueFamilyIndex, 0);

		CreatePipelineAndCommandBuffers();

		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(m_swapchain.m_images.size(), VK_NULL_HANDLE);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			imageAvailableSemaphores[i] = m_logicalDevice.SemaphoreCreate(0);
			renderFinishedSemaphores[i] = m_logicalDevice.SemaphoreCreate(0);
			inFlightFences[i] = m_logicalDevice.FenceCreate(VK_FENCE_CREATE_SIGNALED_BIT);
		}

	}

	void RenderEngineVulkan::OnPostMainLoop(Event::EPostMainLoop& e)
	{
		LINA_TRACE("[Render Engine Vulkan] -> Shutdown");

		m_logicalDevice.DeviceWait();
		CleanupSwapchain();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_logicalDevice.SemaphoreDestroy(imageAvailableSemaphores[i]);
			m_logicalDevice.SemaphoreDestroy(renderFinishedSemaphores[i]);
			m_logicalDevice.FenceDestroy(inFlightFences[i]);
		}

		m_logicalDevice.CommandPoolDestroy(pool);
		m_logicalDevice.DeviceDestroy();

		if (m_vulkanData.m_surface)
		{
			vkDestroySurfaceKHR(m_vulkanData.m_instance, m_vulkanData.m_surface, nullptr);
			m_vulkanData.m_surface = VK_NULL_HANDLE;
		}

		m_loader.Unload();
		m_window.Terminate();
	}


	void RenderEngineVulkan::OnWindowResize(Event::EWindowResized& e)
	{
		m_windowResized = true;
	}

	void RenderEngineVulkan::Tick()
	{
		PROFILER_FUNC();

		m_logicalDevice.FenceWait(inFlightFences[currentFrame], UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_logicalDevice.m_handle, m_swapchain.m_handle, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			LINA_ERR("[Render Engine Vulkan] -> Failed to acquire swapchain image!");
			return;
		}

		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
			m_logicalDevice.FenceWait(imagesInFlight[imageIndex], UINT64_MAX);

		imagesInFlight[imageIndex] = inFlightFences[currentFrame];

		std::vector<VkSemaphore> waitSemaphores = { imageAvailableSemaphores[currentFrame] };
		std::vector<VkSemaphore> signalSemaphores = { renderFinishedSemaphores[currentFrame] };

		QueueSubmitInfo submitInfo
		{
			m_vulkanData.m_graphicsQueue,
			1,
			waitSemaphores,
			{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
			{ commandBuffers[imageIndex] },
			signalSemaphores,
			inFlightFences[currentFrame]
		};

		m_logicalDevice.FenceReset(inFlightFences[currentFrame]);
		m_logicalDevice.QueueSubmit(submitInfo);

		VkSwapchainKHR swapChains[] = { m_swapchain.m_handle };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores.data();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pResults = nullptr;
		vkQueuePresentKHR(m_vulkanData.m_presentationQueue, &presentInfo);

		if (m_windowResized || result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			m_windowResized = false;
			RecreateSwapchain();
			return;
		}
		else if (result != VK_SUCCESS)
		{
			LINA_ERR("[Render Engine Vulkan] -> Failed to acquire swapchain image!");
			return;
		}

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	}

	void RenderEngineVulkan::Render()
	{
		PROFILER_FUNC();

		m_eventSys->Trigger<Event::EPreRender>();
		m_eventSys->Trigger<Event::EPostRender>();
		m_eventSys->Trigger<Event::EFinalizePostRender>();
	}

	void RenderEngineVulkan::CleanupSwapchain()
	{
		for (auto& fb : framebuffers)
			m_logicalDevice.FramebufferDestroy(fb);

		m_logicalDevice.CommandBufferFree(commandBuffers, pool);

		m_logicalDevice.PipelineDestroy(pipeline);
		m_logicalDevice.PipelineDestroyLayout(layout);
		m_logicalDevice.RenderPassDestroy(rp);

		// Finally, the swapchain.
		m_swapchain.Destroy(m_vulkanData.m_logicalDevice);
	}


	void RenderEngineVulkan::CreatePipelineAndCommandBuffers()
	{

		Resources::ShaderResource* vertShader = m_resources->GetShaderResource(StringID("Resources/Shaders/testTriangle.vert").value());
		Resources::ShaderResource* fragShader = m_resources->GetShaderResource(StringID("Resources/Shaders/testTriangle.frag").value());
		VkShaderModule vertModule = m_logicalDevice.ShaderModuleCreate(vertShader->GetData());
		VkShaderModule fragModule = m_logicalDevice.ShaderModuleCreate(fragShader->GetData());

		VkPipelineShaderStageCreateInfo vinfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_VERTEX_BIT,
			vertModule,
			"main",
			nullptr
		};

		VkPipelineShaderStageCreateInfo finfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			fragModule,
			"main",
			nullptr
		};

		layout = m_logicalDevice.PipelineCreateLayout();
		rp = m_logicalDevice.RenderPassCreateDefault(&m_swapchain);
		pipeline = m_logicalDevice.PipelineCreateDefault(&m_swapchain, layout, rp, vinfo, finfo);

		framebuffers.clear();
		framebuffers.resize(m_swapchain.m_imageViews.size());

		for (int i = 0; i < m_swapchain.m_imageViews.size(); ++i)
			framebuffers[i] = m_logicalDevice.FramebufferCreate(&m_swapchain, rp, 1, &m_swapchain.m_imageViews[i]);

		commandBuffers.clear();
		commandBuffers.resize(framebuffers.size());
		commandBuffers = m_logicalDevice.CommandBufferCreate(pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, framebuffers.size());

		for (int i = 0; i < commandBuffers.size(); ++i)
		{
			m_logicalDevice.CommandBufferBegin(commandBuffers[i], 0, nullptr);

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			VkRect2D rpExtent;
			rpExtent.offset = { 0,0 };
			rpExtent.extent = m_swapchain.m_imagesSize;

			VkRenderPassBeginInfo renderPassInfo
			{
				VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				nullptr,
				rp,
				framebuffers[i],
				rpExtent,
				1,
				&clearColor
			};

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
			vkCmdEndRenderPass(commandBuffers[i]);
			m_logicalDevice.CommandBufferEnd(commandBuffers[i]);
		}

		m_logicalDevice.ShaderModuleDestroy(vertModule);
		m_logicalDevice.ShaderModuleDestroy(fragModule);

	}

	void RenderEngineVulkan::RecreateSwapchain()
	{
		m_logicalDevice.DeviceWait();
		CleanupSwapchain();

		// Create a swapchain.
		SwapchainData data;
		data.vulkanData = &m_vulkanData;
		data.windowProps = &m_window.m_windowProperties;
		m_swapchain.Create(data);

		CreatePipelineAndCommandBuffers();
	}
}