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

/*
Class: VulkanLogicalDevice

Wrapper for VkDevice, along with an interface for executing device level functions.

Timestamp: 12/28/2020 3:11:12 AM
*/

#pragma once

#ifndef VulkanLogicalDevice_HPP
#define VulkanLogicalDevice_HPP

// Headers here.
#include "Core/Backend/Vulkan/VulkanCommon.hpp"
#include <vector>
#include <set>

namespace Lina::Graphics
{
	class VulkanLogicalDevice
	{
		
	public:
		
	private:

		friend class RenderEngineVulkan;

		VulkanLogicalDevice() {};
		~VulkanLogicalDevice() {};

		/* DEVICE FUNCTIONS */
		void GetMemoryProperties();
		bool DeviceWait();

		/* COMMAND POOL FUNCTIONS */
		VkCommandPool CommandPoolCreate(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
		bool CommandPoolReset(VkCommandPool pool, VkCommandPoolResetFlags flags);
		void CommandPoolDestroy(VkCommandPool pool);

		/* COMMAND BUFFER FUNCTIONS */
		std::vector<VkCommandBuffer>& CommandBufferCreate(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count);
		bool CommandBufferBegin(VkCommandBuffer cbuffer, VkCommandBufferUsageFlags usage, SecondaryCommandBufferData* data = nullptr);
		bool CommandBufferEnd(VkCommandBuffer cbuffer);
		bool CommandBufferReset(VkCommandBuffer cbuffer, VkCommandBufferResetFlags resetFlags);
		void CommandBufferFree(std::vector<VkCommandBuffer>& buffers, VkCommandPool pool);

		/* FENCE FUNCTIONS */
		VkFence FenceCreate(VkFenceCreateFlags flags);
		bool FenceWait(const std::vector<VkFence>& fences, VkBool32 waitForAll, uint64_t timeOut = 1000000000);
		bool FenceReset(const std::vector<VkFence>& fences);
		void FenceDestroy(VkFence fence);

		/* SEMAPHORE FUNCTIONS */
		VkSemaphore SemaphoreCreate(VkSemaphoreCreateFlags flags);
		void SemaphoreDestroy(VkSemaphore semaphore);

		/* QUEUE FUNCTIONS */
		bool QueueSubmit(QueueSubmitInfo& submitInfo);
		bool QueueWait(VkQueue queue);

		/* BUFFER FUNCTIONS */
		VkBuffer BufferCreate(VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode);

	private:

		VkPhysicalDevice m_physicalDevice;
		VkDevice m_handle;
		VkPhysicalDeviceMemoryProperties m_memProperties;
		std::set<std::vector<VkCommandBuffer>> m_commandBuffers;
		std::set<VkCommandPool> m_commandPools;
		std::set<VkFence> m_fences;
		std::set<VkSemaphore> m_semaphores;
		std::set<VkBuffer> m_buffers;
	};
}

#endif
