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

#include "Core/Backend/Vulkan/Objects/VulkanLogicalDevice.hpp"
#include "Core/Backend/Vulkan/Utility/VulkanFunctions.hpp"
#include "Core/Backend/Vulkan/Objects/VulkanSwapchain.hpp"
#include "Core/Log.hpp"

namespace Lina::Graphics
{

	/* -------------------- DEVICE FUNCTIONS -------------------- */
	/* -------------------- DEVICE FUNCTIONS -------------------- */
	/* -------------------- DEVICE FUNCTIONS -------------------- */
	/* -------------------- DEVICE FUNCTIONS -------------------- */
	bool VulkanLogicalDevice::DeviceWait()
	{
		return vkDeviceWaitIdle(m_handle);
	}

	void VulkanLogicalDevice::DeviceDestroy()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyDevice(m_handle, nullptr);
			m_handle = VK_NULL_HANDLE;
			LINA_TRACE("[Logical Device] -> Successfuly destroyed logical device.");
		}
	}

	void VulkanLogicalDevice::GetMemoryProperties()
	{
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memProperties);
	}

	/* -------------------- COMMAND POOL FUNCTIONS -------------------- */
	/* -------------------- COMMAND POOL FUNCTIONS -------------------- */
	/* -------------------- COMMAND POOL FUNCTIONS -------------------- */
	/* -------------------- COMMAND POOL FUNCTIONS -------------------- */
	VkCommandPool VulkanLogicalDevice::CommandPoolCreate(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
	{
		VkCommandPoolCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			nullptr,
			flags,
			queueFamilyIndex
		};

		VkCommandPool pool;
		VkResult result = vkCreateCommandPool(m_handle, &createInfo, nullptr, &pool);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Pool] -> Could not create command pool.");
			return VK_NULL_HANDLE;
		}

		return pool;
	}

	bool VulkanLogicalDevice::CommandPoolReset(VkCommandPool pool, VkCommandPoolResetFlags flags)
	{
		VkResult result = vkResetCommandPool(m_handle, pool, flags);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Pool] -> Could not reset command pool.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly resetted command pool.");
		return true;
	}

	void VulkanLogicalDevice::CommandPoolDestroy(VkCommandPool pool)
	{
		vkDestroyCommandPool(m_handle, pool, nullptr);
		pool = VK_NULL_HANDLE;
		LINA_TRACE("[Command Pool] -> Successfuly destroyed a command pool.");
	}

	/* -------------------- COMMAND BUFFER FUNCTIONS -------------------- */
	/* -------------------- COMMAND BUFFER FUNCTIONS -------------------- */
	/* -------------------- COMMAND BUFFER FUNCTIONS -------------------- */
	/* -------------------- COMMAND BUFFER FUNCTIONS -------------------- */
	std::vector<VkCommandBuffer> VulkanLogicalDevice::CommandBufferCreate(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count)
	{
		VkCommandBufferAllocateInfo allocateInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			pool,
			level,
			count
		};

		std::vector<VkCommandBuffer> buffers;
		buffers.resize(count);
		VkResult result = vkAllocateCommandBuffers(m_handle, &allocateInfo, &buffers[0]);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not create command buffer.");
			return {};
		}
		else
			LINA_TRACE("[Command Buffer] -> Successfuly created command buffer.");

		return buffers;
	}
	bool VulkanLogicalDevice::CommandBufferBegin(VkCommandBuffer cbuffer, VkCommandBufferUsageFlags usage, SecondaryCommandBufferData* data)
	{
		VkCommandBufferInheritanceInfo inheritanceInfo;

		if (data != nullptr)
		{
			inheritanceInfo = VkCommandBufferInheritanceInfo{
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
				nullptr,
				data->renderPass,
				data->renderPassIndex,
				data->frameBuffer,
				data->enableOcclusionQuery ? (VkBool32)VK_TRUE : (VkBool32)VK_FALSE,
				data->queryFlags,
				data->pipelineStatistics
			};
		}

		VkCommandBufferBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			usage,
			data == nullptr ? VK_NULL_HANDLE : &inheritanceInfo
		};
		VkResult result = vkBeginCommandBuffer(cbuffer, &beginInfo);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not begin primary command buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly beginning command buffer.");
		return true;
	}


	bool VulkanLogicalDevice::CommandBufferEnd(VkCommandBuffer cbuffer)
	{
		VkResult result = vkEndCommandBuffer(cbuffer);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not end command buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly ended buffer.");
		return true;
	}

	bool VulkanLogicalDevice::CommandBufferReset(VkCommandBuffer cbuffer, VkCommandBufferResetFlags resetFlags)
	{
		// TODO: Table look up. If the pool allocated for this buffer was not allocated with VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT flag
		// a seperate reset can not be performed.

		VkResult result = vkResetCommandBuffer(cbuffer, resetFlags);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could notresetcommand buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly resetted buffer.");
		return true;
	}

	void VulkanLogicalDevice::CommandBufferFree(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool)
	{
		if (buffers.size() > 0)
		{
			uint32_t size = static_cast<uint32_t>(buffers.size());
			vkFreeCommandBuffers(m_handle, pool, size, &buffers[0]);
			buffers.clear();
			LINA_TRACE("[Command Buffer] -> Successfuly freed a command buffer.");
		}
	}

	/* -------------------- FENCE FUNCTIONS -------------------- */
	/* -------------------- FENCE FUNCTIONS -------------------- */
	/* -------------------- FENCE FUNCTIONS -------------------- */
	/* -------------------- FENCE FUNCTIONS -------------------- */
	VkFence VulkanLogicalDevice::FenceCreate(VkFenceCreateFlags flags)
	{
		VkFenceCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			nullptr,
			flags
		};

		VkFence fence;
		VkResult result = vkCreateFence(m_handle, &createInfo, nullptr, &fence);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Fence] -> Could not create a fence.");
			fence = VK_NULL_HANDLE;
			return fence;
		}

		LINA_TRACE("[Fence] -> Successfuly created a fence.");
		return fence;
	}

	bool VulkanLogicalDevice::FencesWait(const std::vector<VkFence>& fences, VkBool32 waitForAll, uint64_t timeOut)
	{
		// We can pass a 0 timeout and check the VkResult to see if the semaphore is available or not.
		VkResult result = vkWaitForFences(m_handle, static_cast<uint32_t>(fences.size()), &fences[0], waitForAll, timeOut);
		return result == VK_TRUE;
	}

	bool VulkanLogicalDevice::FencesReset(const std::vector<VkFence>& fences)
	{
		VkResult result = vkResetFences(m_handle, static_cast<uint32_t>(fences.size()), &fences[0]);
		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Fence] -> Could not reset fences.");
			return false;
		}

		LINA_DEBUG("[Fence] -> Successfuly resetted fences.");
		return true;
	}

	bool VulkanLogicalDevice::FenceWait(VkFence fence, uint64_t timeOut)
	{
		// We can pass a 0 timeout and check the VkResult to see if the semaphore is available or not.
		VkResult result = vkWaitForFences(m_handle, 1, &fence, VK_TRUE, timeOut);
		return result == VK_TRUE;
	}

	bool VulkanLogicalDevice::FenceReset(VkFence fence)
	{
		VkResult result = vkResetFences(m_handle, 1, &fence);
		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Fence] -> Could not reset the fence.");
			return false;
		}

		return true;
	}

	void VulkanLogicalDevice::FenceDestroy(VkFence fence)
	{
		if (fence != VK_NULL_HANDLE)
		{
			vkDestroyFence(m_handle, fence, nullptr);
			fence = VK_NULL_HANDLE;
			LINA_TRACE("[Fence] -> Successfuly destroyed a fence.");
		}
	}

	/* -------------------- SEMAPHORE FUNCTIONS -------------------- */
	/* -------------------- SEMAPHORE FUNCTIONS -------------------- */
	/* -------------------- SEMAPHORE FUNCTIONS -------------------- */
	/* -------------------- SEMAPHORE FUNCTIONS -------------------- */
	VkSemaphore VulkanLogicalDevice::SemaphoreCreate(VkSemaphoreCreateFlags flags)
	{
		VkSemaphoreCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			nullptr,
			flags
		};

		VkSemaphore semaphore;
		VkResult result = vkCreateSemaphore(m_handle, &createInfo, nullptr, &semaphore);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Semaphore] -> Could not create a semaphore.");
			semaphore = VK_NULL_HANDLE;
			return semaphore;
		}

		LINA_TRACE("[Semaphore] -> Successfuly created a semaphore.");
		return semaphore;
	}

	void VulkanLogicalDevice::SemaphoreDestroy(VkSemaphore semaphore)
	{
		if (semaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(m_handle, semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
			LINA_TRACE("[Semaphore] -> Successfuly destroyed a semaphore.");
		}
	}

	bool VulkanLogicalDevice::QueueSubmit(QueueSubmitInfo& submitInfo)
	{
		uint32_t waitSemaphoresSize = static_cast<uint32_t>(submitInfo.waitSemaphores.size());
		uint32_t waitSemaphoreStagesSize = static_cast<uint32_t>(submitInfo.waitSemaphoreStages.size());
		uint32_t commandBuffersSize = static_cast<uint32_t>(submitInfo.commandBuffers.size());
		uint32_t signalSemaphoresSize = static_cast<uint32_t>(submitInfo.signalSemaphores.size());

		VkSubmitInfo info
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			waitSemaphoresSize,
			waitSemaphoresSize == 0 ? nullptr : &submitInfo.waitSemaphores[0],
			waitSemaphoreStagesSize == 0 ? nullptr : &submitInfo.waitSemaphoreStages[0],
			commandBuffersSize,
			commandBuffersSize == 0 ? nullptr : &submitInfo.commandBuffers[0],
			signalSemaphoresSize,
			signalSemaphoresSize == 0 ? nullptr : &submitInfo.signalSemaphores[0]
		};

		VkResult result = vkQueueSubmit(submitInfo.submitQueue, submitInfo.submitCount, &info, submitInfo.fence);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Queue] -> Could not submit the queue.");
			return false;
		}

		return true;
	}

	bool VulkanLogicalDevice::QueueWait(VkQueue queue)
	{
		VkResult result = vkQueueWaitIdle(queue);
		return result == VK_SUCCESS;
	}

	/* -------------------- BUFFER FUNCTIONS -------------------- */
	/* -------------------- BUFFER FUNCTIONS -------------------- */
	/* -------------------- BUFFER FUNCTIONS -------------------- */
	/* -------------------- BUFFER FUNCTIONS -------------------- */
	VkBuffer VulkanLogicalDevice::BufferCreate(VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode)
	{
		VkBufferCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			nullptr,
			flags,
			size,
			usage,
			sharingMode
		};

		VkBuffer buffer;
		VkResult result = vkCreateBuffer(m_handle, &createInfo, nullptr, &buffer);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Buffer] -> Could not create a buffer.");
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Buffer] -> Successfuly created a buffer.");
		return buffer;
	}

	/* -------------------- SHADER MODULE FUNCTIONS -------------------- */
	/* -------------------- SHADER MODULE FUNCTIONS -------------------- */
	/* -------------------- SHADER MODULE FUNCTIONS -------------------- */
	/* -------------------- SHADER MODULE FUNCTIONS -------------------- */
	VkShaderModule VulkanLogicalDevice::ShaderModuleCreate(const std::vector<uint32_t>& buffer)
	{
		VkShaderModuleCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,
			buffer.size() * sizeof(uint32_t),
			buffer.data()
		};

		VkShaderModule shaderModule;
		VkResult result = vkCreateShaderModule(m_handle, &createInfo, nullptr, &shaderModule);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Shader Module] -> Could not create a shader module.");
			shaderModule = VK_NULL_HANDLE;
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Shader Module] -> Successfuly created  a shader module.");
		return shaderModule;
	}

	void VulkanLogicalDevice::ShaderModuleDestroy(VkShaderModule module)
	{
		vkDestroyShaderModule(m_handle, module, nullptr);
		LINA_TRACE("[Shader Module] -> Successfuly deleted a shader module.");
	}

	/* -------------------- GRAPHICS PIPELINE FUNCTIONS -------------------- */
	/* -------------------- GRAPHICS PIPELINE FUNCTIONS -------------------- */
	/* -------------------- GRAPHICS PIPELINE FUNCTIONS -------------------- */
	/* -------------------- GRAPHICS PIPELINE FUNCTIONS -------------------- */
	VkPipeline VulkanLogicalDevice::PipelineCreateDefault(VulkanSwapchain* swapchain, VkPipelineLayout layout, VkRenderPass rp, VkPipelineShaderStageCreateInfo& vertShaderStageInfo, VkPipelineShaderStageCreateInfo& fragShaderStageInfo)
	{
		VkPipelineVertexInputStateCreateInfo vertexInputInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			0,
			nullptr,
			0,
			nullptr
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssembly
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE
		};

		VkViewport viewport
		{
			0.0f,
			0.0f,
			(float)swapchain->m_imagesSize.width,
			(float)swapchain->m_imagesSize.height,
			0.0f,
			1.0f
		};

		VkRect2D scissor
		{
			{0, 0},
			swapchain->m_imagesSize
		};

		VkPipelineViewportStateCreateInfo viewportState
		{
			 VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			 nullptr,
			 0,
			 1,
			 &viewport,
			 1,
			 &scissor
		};

		VkPipelineRasterizationStateCreateInfo rasterizer
		{
			 VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			 nullptr,
			 0,
			 VK_FALSE,
			 VK_FALSE,
			 VK_POLYGON_MODE_FILL,
			 VK_CULL_MODE_BACK_BIT,
			 VK_FRONT_FACE_CLOCKWISE,
			 VK_FALSE,
			 0.0f,
			 0.0f,
			 0.0f,
			 1.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampling
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_SAMPLE_COUNT_1_BIT,
			VK_FALSE,	// sample shading?
			1.0f,
			nullptr,
			VK_FALSE,	// alpha to coverage?
			VK_FALSE	// alpha to one?
		};

		VkPipelineColorBlendAttachmentState colorBlendAttachment
		{
			VK_FALSE,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
		};

		VkPipelineColorBlendStateCreateInfo colorBlending
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			1,
			&colorBlendAttachment,
			{0.0f, 0.0f, 0.0f, 0.0f}
		};

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamicState
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			nullptr,
			0,
			2,
			dynamicStates
		};

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
		VkGraphicsPipelineCreateInfo pipelineInfo
		{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			nullptr,
			0,
			2,
			shaderStages,
			&vertexInputInfo,
			&inputAssembly,
			nullptr,
			&viewportState,
			&rasterizer,
			&multisampling,
			nullptr,
			&colorBlending,
			nullptr,
			layout,
			rp,
			0,
			VK_NULL_HANDLE,
			-1
		};

		VkPipeline graphicsPipeline;
		VkResult result = vkCreateGraphicsPipelines(m_handle, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
		
		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Pipeline] -> Could not create a graphics pipeline.");
			graphicsPipeline = VK_NULL_HANDLE;
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Pipeline] -> Successfuly created a graphics pipeline.");
		return graphicsPipeline;
	}


	VkPipelineLayout VulkanLogicalDevice::PipelineCreateLayout()
	{
		VkPipelineLayout pipelineLayout;
		VkPipelineLayoutCreateInfo pipelineLayoutInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			0,
			nullptr,
			0,
			nullptr
		};

		VkResult result = vkCreatePipelineLayout(m_handle, &pipelineLayoutInfo, nullptr, &pipelineLayout);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Pipeline Layout] -> Could not create a pipeline layout.");
			pipelineLayout = VK_NULL_HANDLE;
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Pipeline Layout] -> Successfuly created  a pipeline layout.");
		return pipelineLayout;
	}

	void VulkanLogicalDevice::PipelineDestroyLayout(VkPipelineLayout layout)
	{
		if (layout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(m_handle, layout, nullptr);
			layout = VK_NULL_HANDLE;
			LINA_TRACE("[Pipeline layout] -> Successfuly destroyed a pipeline layout.");
		}
	}

	void VulkanLogicalDevice::PipelineDestroy(VkPipeline pipeline)
	{
		if (pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(m_handle, pipeline, nullptr);
			pipeline = VK_NULL_HANDLE;
			LINA_TRACE("[Pipeline] -> Successfuly destroyed a pipeline.");
		}
	}

	/* -------------------- RENDER PASS FUNCTIONS -------------------- */
	/* -------------------- RENDER PASS FUNCTIONS -------------------- */
	/* -------------------- RENDER PASS FUNCTIONS -------------------- */
	/* -------------------- RENDER PASS FUNCTIONS -------------------- */
	VkRenderPass VulkanLogicalDevice::RenderPassCreateDefault(VulkanSwapchain* swapchain)
	{

		VkAttachmentDescription colorAttachment
		{
			0,
			swapchain->m_imageFormat,
			VK_SAMPLE_COUNT_1_BIT,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colorAttachmentRef
		{
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency
		{
			VK_SUBPASS_EXTERNAL,
			0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			0
		};

		VkRenderPass renderPass;
		VkRenderPassCreateInfo renderPassInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			0,
			1,
			&colorAttachment,
			1,
			&subpass,
			1,
			&dependency
		};

		VkResult result = vkCreateRenderPass(m_handle, &renderPassInfo, nullptr, &renderPass);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Render Pass] -> Could not create a render pass.");
			renderPass = VK_NULL_HANDLE;
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Render Pass] -> Successfuly created a render pass.");
		return renderPass;
	}

	void VulkanLogicalDevice::RenderPassDestroy(VkRenderPass renderPass)
	{
		if (renderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(m_handle, renderPass, nullptr);
			renderPass = VK_NULL_HANDLE;
			LINA_TRACE("[Render Pass] -> Successfuly destroyed a render pass.");
		}
	}


	VkFramebuffer VulkanLogicalDevice::FramebufferCreate(VulkanSwapchain* swapchain, VkRenderPass renderPass, uint32_t attachmentCount, VkImageView* attachments)
	{
		VkFramebufferCreateInfo info
		{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			renderPass,
			attachmentCount,
			attachments,
			swapchain->m_imagesSize.width,
			swapchain->m_imagesSize.height,
			1
		};

		VkFramebuffer framebuffer;
		VkResult  result = vkCreateFramebuffer(m_handle, &info, nullptr, &framebuffer);
		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Framebuffer] -> Could not create a frame buffer.");
			framebuffer = VK_NULL_HANDLE;
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Render Pass] -> Successfuly created a framebuffer.");
		return framebuffer;
	}

	void VulkanLogicalDevice::FramebufferDestroy(VkFramebuffer frameBuffer)
	{
		if (frameBuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(m_handle, frameBuffer, nullptr);
			frameBuffer = VK_NULL_HANDLE;
			LINA_TRACE("[Frame Buffer] -> Successfuly destroyed a frame buffer.");
		}
	}


}