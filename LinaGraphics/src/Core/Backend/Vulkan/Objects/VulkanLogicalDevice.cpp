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
#include "Core/Log.hpp"
namespace Lina::Graphics
{
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

		m_commandPools.push_back(pool);
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

	/* -------------------- COMMAND BUFFER FUNCTIONS -------------------- */
	/* -------------------- COMMAND BUFFER FUNCTIONS -------------------- */
	/* -------------------- COMMAND BUFFER FUNCTIONS -------------------- */
	/* -------------------- COMMAND BUFFER FUNCTIONS -------------------- */
	std::vector<VkCommandBuffer>& VulkanLogicalDevice::CommandBufferCreate(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count)
	{
		VkCommandBufferAllocateInfo allocateInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			pool,
			level,
			count
		};

		m_primaryCommandBuffers.resize(count);
		VkResult result = vkAllocateCommandBuffers(m_handle, &allocateInfo, &m_primaryCommandBuffers[0]);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not create command buffer.");
			m_primaryCommandBuffers.clear();
			m_primaryCommandBuffers.resize(0);
		}
		else
			LINA_TRACE("[Command Buffer] -> Successfuly created command buffer.");

		return m_primaryCommandBuffers;
	}
	bool VulkanLogicalDevice::CommandBufferBeginPrimary(uint32_t index, VkCommandBufferUsageFlags usage)
	{
		VkCommandBufferBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			usage,
			VK_NULL_HANDLE
		};
		VkResult result = vkBeginCommandBuffer(m_primaryCommandBuffers[index], &beginInfo);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not begin primary command buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly beginning command buffer.");
		return true;
	}

	bool VulkanLogicalDevice::CommandBufferBeginSecondary(uint32_t index, VkCommandBufferUsageFlags usage, SecondaryCommandBufferData& data)
	{
		VkCommandBufferInheritanceInfo inheritanceInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			nullptr,
			data.renderPass,
			data.renderPassIndex,
			data.frameBuffer,
			data.enableOcclusionQuery ? VK_TRUE : VK_FALSE,
			data.queryFlags,
			data.pipelineStatistics
		};

		VkCommandBufferBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			usage,
			&inheritanceInfo
		};
		VkResult result = vkBeginCommandBuffer(m_primaryCommandBuffers[index], &beginInfo);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not begin secondary command buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly beginning secondary buffer.");
		return true;
	}

	bool VulkanLogicalDevice::CommandBufferEnd(uint32_t index)
	{
		VkResult result = vkEndCommandBuffer(m_primaryCommandBuffers[index]);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not end command buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly ended buffer.");
		return true;
	}

	bool VulkanLogicalDevice::CommandBufferReset(uint32_t index, VkCommandBufferResetFlags resetFlags)
	{
		// TODO: Table look up. If the pool allocated for this buffer was not allocated with VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT flag
		// a seperate reset can not be performed.

		VkResult result = vkResetCommandBuffer(m_primaryCommandBuffers[index], resetFlags);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could notresetcommand buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly resetted buffer.");
		return true;
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
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Fence] -> Successfuly created a fence.");
		m_fences.push_back(fence);
		return fence;
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
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Semaphore] -> Successfuly created a semaphore.");
		m_semaphores.push_back(semaphore);
		return semaphore;
	}
}