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
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkRenderPass renderPass;
	VkFramebuffer frameBuffer;
	VkFence drawingFence;
	VkSemaphore imageAcquireSemaphore;
	VkSemaphore readyToPresentSemaphore;

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
			m_logicalDevice.GetMemoryProperties();
			m_vulkanHelper.SetReferences(&m_logicalDevice);

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

		commandPool = m_logicalDevice.CommandPoolCreate(m_vulkanData.m_graphicsQueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		std::vector<VkCommandBuffer> commandBuffers;
		m_logicalDevice.CommandBufferCreate(commandBuffers, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
		commandBuffer = commandBuffers[0];

		drawingFence = m_logicalDevice.FenceCreate(VK_FENCE_CREATE_SIGNALED_BIT);
		imageAcquireSemaphore = m_logicalDevice.SemaphoreCreate(0);
		readyToPresentSemaphore = m_logicalDevice.SemaphoreCreate(0);

		// Render pass
		std::vector<VkAttachmentDescription> attachment_descriptions = {
		  {
			0,                                // VkAttachmentDescriptionFlags     flags
			m_swapchain.m_imageFormat,                 // VkFormat                         format
			VK_SAMPLE_COUNT_1_BIT,            // VkSampleCountFlagBits            samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,      // VkAttachmentLoadOp               loadOp
			VK_ATTACHMENT_STORE_OP_STORE,     // VkAttachmentStoreOp              storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // VkAttachmentLoadOp               stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE, // VkAttachmentStoreOp              stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,        // VkImageLayout                    initialLayout
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR   // VkImageLayout                    finalLayout
		  }
		};

		std::vector<SubpassParameters> subpass_parameters = {
		  {
			VK_PIPELINE_BIND_POINT_GRAPHICS,            // VkPipelineBindPoint                  PipelineType
			{},                                         // std::vector<VkAttachmentReference>   InputAttachments
			{
			  {                                         // std::vector<VkAttachmentReference>   ColorAttachments
				0,                                        // uint32_t                             attachment
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // VkImageLayout                        layout
			  }
			},
			{},                                         // std::vector<VkAttachmentReference>   ResolveAttachments
			nullptr,                                    // VkAttachmentReference const        * DepthStencilAttachment
			{}                                          // std::vector<uint32_t>                PreserveAttachments
		  }
		};

		std::vector<VkSubpassDependency> subpass_dependencies = {
		  {
			VK_SUBPASS_EXTERNAL,                            // uint32_t                   srcSubpass
			0,                                              // uint32_t                   dstSubpass
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       srcStageMask
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       dstStageMask
			VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              srcAccessMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              dstAccessMask
			VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
		  },
		  {
			0,                                              // uint32_t                   srcSubpass
			VK_SUBPASS_EXTERNAL,                            // uint32_t                   dstSubpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       srcStageMask
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       dstStageMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              srcAccessMask
			VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              dstAccessMask
			VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
		  }
		};

		renderPass = m_logicalDevice.RenderPassCreate(attachment_descriptions, subpass_parameters, subpass_dependencies);

	}

	void RenderEngineVulkan::OnPostMainLoop(Event::EPostMainLoop& e)
	{
		LINA_TRACE("[Render Engine Vulkan] -> Shutdown");

		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
		VkRenderPass renderPass;
		VkFramebuffer frameBuffer;
		VkFence drawingFence;
		VkSemaphore imageAcquireSemaphore;
		VkSemaphore readyToPresentSemaphore;

		m_logicalDevice.CommandPoolDestroy(commandPool);
		m_logicalDevice.RenderPassDestroy(renderPass);
		m_logicalDevice.FramebufferDestroy(frameBuffer);
		m_logicalDevice.FenceDestroy(drawingFence);
		m_logicalDevice.SemaphoreDestroy(imageAcquireSemaphore);
		m_logicalDevice.SemaphoreDestroy(readyToPresentSemaphore);

		m_logicalDevice.DeviceWait();

		// Finally, the swapchain.
		m_swapchain.Destroy(m_vulkanData.m_logicalDevice);

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
		while (e.m_windowProps.m_width == 0|| e.m_windowProps.m_height == 0)
			m_window.WaitEvents();
	
		m_logicalDevice.DeviceWait();

		SwapchainData data;
		data.vulkanData = &m_vulkanData;
		data.windowProps = &e.m_windowProps;
		m_swapchain.Create(data);
	}

	void RenderEngineVulkan::Tick()
	{
		PROFILER_FUNC();

		m_logicalDevice.FenceWait(drawingFence, VK_FALSE, 5000000000);
		m_logicalDevice.FenceReset(drawingFence);
		uint32_t imageIndex = m_swapchain.AcquireNextImage(m_logicalDevice.m_handle, imageAcquireSemaphore, VK_NULL_HANDLE);
	
		frameBuffer = m_logicalDevice.FramebufferCreate(&m_swapchain, renderPass, { m_swapchain.m_imageViews[imageIndex] }, 1);
		m_logicalDevice.CommandBufferBegin(commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		if (m_vulkanData.m_presentationQueueFamilyIndex != m_vulkanData.m_graphicsQueueFamilyIndex)
		{
			ImageTransition image_transition_before_drawing = 
			{
				m_swapchain.m_images[imageIndex],              // VkImage              Image
				VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags        CurrentAccess
				VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags        NewAccess
				VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout        CurrentLayout
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,   // VkImageLayout        NewLayout
				m_vulkanData.m_presentationQueueFamilyIndex,                   // uint32_t             CurrentQueueFamily
				m_vulkanData.m_graphicsQueueFamilyIndex,                  // uint32_t             NewQueueFamily
				VK_IMAGE_ASPECT_COLOR_BIT                   // VkImageAspectFlags   Aspect
			};

			m_logicalDevice.CommandBufferSetImageMemoryBarriers(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				{ image_transition_before_drawing });
		}

		m_logicalDevice.CommandBufferBeginRenderPass(commandBuffer, renderPass, frameBuffer, { {0,0}, m_swapchain.m_imagesSize }, { {0.2f, 0.5f, 0.8f, 1.0f} }, VK_SUBPASS_CONTENTS_INLINE);
		m_logicalDevice.CommandBufferEndRenderPass(commandBuffer);

		if (m_vulkanData.m_presentationQueueFamilyIndex != m_vulkanData.m_graphicsQueueFamilyIndex)
		{
			ImageTransition image_transition_before_present = 
			{
				m_swapchain.m_images[imageIndex],              // VkImage              Image
				VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags        CurrentAccess
				VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags        NewAccess
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,            // VkImageLayout        CurrentLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,            // VkImageLayout        NewLayout
				m_vulkanData.m_presentationQueueFamilyIndex,                   // uint32_t             CurrentQueueFamily
				m_vulkanData.m_graphicsQueueFamilyIndex,                  // uint32_t             NewQueueFamily
				VK_IMAGE_ASPECT_COLOR_BIT                   // VkImageAspectFlags   Aspect
			};

			m_logicalDevice.CommandBufferSetImageMemoryBarriers(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				{ image_transition_before_present });
		}

		m_logicalDevice.CommandBufferEnd(commandBuffer);

		WaitSemaphoreInfo waitSemaphoreInfo = 
		{
			imageAcquireSemaphore,     
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT 
		};

		m_logicalDevice.QueueSubmit(m_vulkanData.m_graphicsQueue, { waitSemaphoreInfo }, { commandBuffer }, { readyToPresentSemaphore }, drawingFence);

		PresentInfo presentInfo = 
		{
			m_swapchain.m_handle, 
			imageIndex      
		};

		m_logicalDevice.QueuePresent(m_vulkanData.m_presentationQueue, { readyToPresentSemaphore }, { presentInfo });
	}

	void RenderEngineVulkan::Render()
	{
		PROFILER_FUNC();

		m_eventSys->Trigger<Event::EPreRender>();
		m_eventSys->Trigger<Event::EPostRender>();
		m_eventSys->Trigger<Event::EFinalizePostRender>();
	}

}