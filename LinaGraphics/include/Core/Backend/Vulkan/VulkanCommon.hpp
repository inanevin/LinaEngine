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
Class: VulkanCommon



Timestamp: 12/26/2020 9:39:14 PM
*/

#pragma once

#ifndef VulkanCommon_HPP
#define VulkanCommon_HPP


#include "Core/MacroDetection.hpp"
#include <cstdint>
#include <vector>

#define VK_NO_PROTOTYPES
#ifdef LINA_WINDOWS 
#define VK_USE_PLATFORM_WIN32_KHR
#elif LINA_LINUX
#ifdef LINA_LINUX_XLIB
#elif LINA_LINUX_XCB
#endif
#endif
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    struct QueueInfo 
    {
        uint32_t m_familyIndex;
        std::vector<float> m_priorities;
    };

	struct VulkanData
	{
		VkInstance m_instance;
		VkDevice m_logicalDevice;
		VkPhysicalDevice m_physicalDevice;
		VkSurfaceKHR m_surface;
		VkQueue m_graphicsQueue;
		VkQueue m_computeQueue;
		VkQueue m_presentationQueue;
		VkPresentModeKHR m_presentMode;
		uint32_t m_graphicsQueueFamilyIndex;
		uint32_t m_computeQueueFamilyIndex;
		uint32_t m_presentationQueueFamilyIndex;
	};

	struct SecondaryCommandBufferData
	{
		VkRenderPass renderPass = VK_NULL_HANDLE;
		uint32_t renderPassIndex = 0;
		VkFramebuffer frameBuffer = VK_NULL_HANDLE;
		bool enableOcclusionQuery = false;
		VkQueryControlFlags	queryFlags;
		VkQueryPipelineStatisticFlags pipelineStatistics;
	};

	struct QueueSubmitInfo
	{
		VkQueue submitQueue = VK_NULL_HANDLE;
		uint32_t submitCount = 0;
		std::vector<VkSemaphore> waitSemaphores;
		std::vector<VkPipelineStageFlags> waitSemaphoreStages;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkSemaphore> signalSemaphores;
		VkFence fence = VK_NULL_HANDLE;
	};
}


#endif
