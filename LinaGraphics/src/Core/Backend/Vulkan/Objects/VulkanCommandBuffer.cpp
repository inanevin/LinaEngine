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

#include "Core/Backend/Vulkan/Objects/VulkanCommandBuffer.hpp"
#include "Core/Backend/Vulkan/Utility/VulkanFunctions.hpp"
#include "Core/Log.hpp"

namespace Lina::Graphics
{
	std::vector<VkCommandBuffer>& VulkanCommandBuffer::Create(VkDevice logicalDevice, VkCommandPool pool, VkCommandBufferLevel level, uint32_t count)
	{
		VkCommandBufferAllocateInfo allocateInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			pool,
			level,
			count
		};

		m_handles.resize(count);
		VkResult result = vkAllocateCommandBuffers(logicalDevice, &allocateInfo, &m_handles[0]);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not create command buffer.");
		}
		else
			LINA_TRACE("[Command Buffer] -> Successfuly created command buffer.");

		return m_handles;
	}
	bool VulkanCommandBuffer::BeginPrimary(VkDevice logicalDevice, uint32_t index, VkCommandBufferUsageFlags usage)
	{
		VkCommandBufferBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			usage,
			VK_NULL_HANDLE
		};
		VkResult result = vkBeginCommandBuffer(m_handles[index], &beginInfo);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not begin primary command buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly beginning command buffer.");
		return true;
	}

	bool VulkanCommandBuffer::BeginSecondary(VkDevice logicalDevice, uint32_t index, VkCommandBufferUsageFlags usage, SecondaryCommandBufferData& data)
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
		VkResult result = vkBeginCommandBuffer(m_handles[index], &beginInfo);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not begin secondary command buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly beginning secondary buffer.");
		return true;
	}

	bool VulkanCommandBuffer::End(uint32_t index)
	{
		VkResult result = vkEndCommandBuffer(m_handles[index]);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could not end command buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly ended buffer.");
		return true;
	}

	bool VulkanCommandBuffer::Reset(uint32_t index, VkCommandBufferResetFlags resetFlags)
	{
		// TODO: Table look up. If the pool allocated for this buffer was not allocated with VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT flag
		// a seperate reset can not be performed.

		VkResult result = vkResetCommandBuffer(m_handles[index], resetFlags);

		if (result != VK_SUCCESS)
		{
			LINA_ERR("[Command Buffer] -> Could notresetcommand buffer.");
			return false;
		}

		LINA_TRACE("[Command Buffer] -> Successfuly resetted buffer.");
		return true;
	}

}