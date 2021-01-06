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
Class: VulkanHelper

Defines common functions for creating & organizing vulkan structures.

Timestamp: 1/7/2021 12:01:01 AM
*/

#pragma once

#ifndef VulkanHelper_HPP
#define VulkanHelper_HPP

// Headers here.
#include "Core/Backend/Vulkan/Objects/VulkanLogicalDevice.hpp"


namespace Lina::Graphics
{
	class VulkanHelper
	{
		
	public:
		
	private:

		friend class RenderEngineVulkan;

		VulkanHelper() {};
		~VulkanHelper() {};
	
		inline void SetReferences(VulkanLogicalDevice* logicalDevice)
		{
			m_logicalDevice = logicalDevice;
		}

		/* RENDER PASS FUNCTIONS */
		VkRenderPass RenderPassCreateGeometryAndPostProcess();


	private:
	
		VulkanLogicalDevice* m_logicalDevice = nullptr;
	};
}

#endif
