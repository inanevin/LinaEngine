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


		/* COMMAND POOL FUNCTIONS */
		VkCommandPool CommandPoolCreate(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
		bool CommandPoolReset(VkCommandPool pool, VkCommandPoolResetFlags flags);

		/* COMMAND BUFFER FUNCTIONS */
		std::vector<VkCommandBuffer>& CommandBufferCreate(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count);
		bool CommandBufferBeginPrimary(uint32_t index, VkCommandBufferUsageFlags usage);
		bool CommandBufferBeginSecondary(uint32_t index, VkCommandBufferUsageFlags usage, SecondaryCommandBufferData& data);
		bool CommandBufferEnd(uint32_t index);
		bool CommandBufferReset(uint32_t index, VkCommandBufferResetFlags resetFlags);

		/* FENCE FUNCTIONS */
		VkFence FenceCreate(VkFenceCreateFlags flags);

		/* SEMAPHORE FUNCTIONS */
		VkSemaphore SemaphoreCreate(VkSemaphoreCreateFlags flags);


	private:

		VkDevice m_handle;
		std::vector<VkCommandBuffer> m_primaryCommandBuffers;
		std::vector<VkCommandBuffer> m_secondaryCommandBuffers;
		std::vector<VkCommandPool> m_commandPools;
		std::vector<VkFence> m_fences;
		std::vector<VkSemaphore> m_semaphores;
	};
}

#endif
