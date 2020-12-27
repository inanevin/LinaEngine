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
Class: VulkanCommandPool

Wrapper for Vulkan Command Pools.

Timestamp: 12/28/2020 12:40:52 AM
*/

#pragma once

#ifndef VulkanCommandPool_HPP
#define VulkanCommandPool_HPP

// Headers here.
#include "Core/Backend/Vulkan/VulkanCommon.hpp"

namespace Lina::Graphics
{
	class VulkanCommandPool
	{
		
	private:
		
		friend class VulkanHandler;
		friend class RenderEngineVulkan;

		VulkanCommandPool() {};
		~VulkanCommandPool() {};
		
		VkCommandPool Create(VkDevice logicalDevice, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);

	private:
	
		VkCommandPool m_handle = VK_NULL_HANDLE;

	};
}

#endif
