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

	/* -------------------- DEVICE FUNCTIONS -------------------- */
	/* -------------------- DEVICE FUNCTIONS -------------------- */
	/* -------------------- DEVICE FUNCTIONS -------------------- */
	/* -------------------- DEVICE FUNCTIONS -------------------- */
	bool VulkanLogicalDevice::DeviceWait()
	{
		return vkDeviceWaitIdle(m_handle);
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

		m_commandPools.insert(pool);
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
		m_commandPools.erase(pool);
		vkDestroyCommandPool(m_handle, pool, nullptr);
		pool = VK_NULL_HANDLE;

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

		std::vector<VkCommandBuffer> buffers;
		buffers.resize(count);
		VkResult result = vkAllocateCommandBuffers(m_handle, &allocateInfo, &buffers[0]);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not create command buffer.");
			m_buffers.clear();
		}
		else
			LINA_TRACE("[Command Buffer] -> Successfuly created command buffer.");

		m_commandBuffers.insert(buffers);
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
		m_commandBuffers.erase(buffers);
		if (buffers.size() > 0)
		{
			uint32_t size = static_cast<uint32_t>(buffers.size());
			vkFreeCommandBuffers(m_handle, pool, size, &buffers[0]);
			buffers.clear();
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
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Fence] -> Successfuly created a fence.");
		m_fences.insert(fence);
		return fence;
	}

	bool VulkanLogicalDevice::FenceWait(const std::vector<VkFence>& fences, VkBool32 waitForAll, uint64_t timeOut)
	{
		// We can pass a 0 timeout and check the VkResult to see if the semaphore is available or not.
		VkResult result = vkWaitForFences(m_handle, static_cast<uint32_t>(fences.size()), &fences[0], waitForAll, timeOut);

		return result == VK_TRUE;
	}

	bool VulkanLogicalDevice::FenceReset(const std::vector<VkFence>& fences)
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

	void VulkanLogicalDevice::FenceDestroy(VkFence fence)
	{
		m_fences.erase(fence);
		if (fence != VK_NULL_HANDLE)
		{
			vkDestroyFence(m_handle, fence, nullptr);
			fence = VK_NULL_HANDLE;
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
			return VK_NULL_HANDLE;
		}

		LINA_TRACE("[Semaphore] -> Successfuly created a semaphore.");
		m_semaphores.insert(semaphore);
		return semaphore;
	}

	void VulkanLogicalDevice::SemaphoreDestroy(VkSemaphore semaphore)
	{
		m_semaphores.erase(semaphore);
		if (semaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(m_handle, semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
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

		LINA_TRACE("[Semaphore] -> Successfuly submitted the queue.");
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

		LINA_TRACE("[Semaphore] -> Successfuly created a buffer.");
		m_buffers.insert(buffer);
		return buffer;
	}

}