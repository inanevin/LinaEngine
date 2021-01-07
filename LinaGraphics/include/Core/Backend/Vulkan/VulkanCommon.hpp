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
		VkInstance		 m_instance;
		VkDevice		 m_logicalDevice;
		VkPhysicalDevice m_physicalDevice;
		VkSurfaceKHR	 m_surface;
		VkQueue			 m_graphicsQueue;
		VkQueue			 m_computeQueue;
		VkQueue			 m_presentationQueue;
		VkPresentModeKHR m_presentMode;
		uint32_t		 m_graphicsQueueFamilyIndex;
		uint32_t		 m_computeQueueFamilyIndex;
		uint32_t		 m_presentationQueueFamilyIndex;
	};

	struct SecondaryCommandBufferData
	{
		VkRenderPass					m_renderPass = VK_NULL_HANDLE;
		uint32_t						m_renderPassIndex = 0;
		VkFramebuffer					m_frameBuffer = VK_NULL_HANDLE;
		bool							m_enableOcclusionQuery = false;
		VkQueryControlFlags				m_queryFlags;
		VkQueryPipelineStatisticFlags	m_pipelineStatistics;
	};

	

	struct BufferTransition 
	{
		VkBuffer        m_buffer;
		VkAccessFlags   m_currentAccess;
		VkAccessFlags   m_newAccess;
		uint32_t        m_currentQueueFamily;
		uint32_t        m_newQueueFamily;
	};

	struct ImageTransition {
		VkImage             m_image;
		VkAccessFlags       m_currentAccess;
		VkAccessFlags       m_newAccess;
		VkImageLayout       m_currentLayout;
		VkImageLayout       m_newLayout;
		uint32_t            m_currentQueueFamily;
		uint32_t            m_newQueueFamily;
		VkImageAspectFlags  m_aspect;
	};

	struct SamplerParameters
	{
		VkFilter                m_magFilter = VK_FILTER_NEAREST;
		VkFilter                m_minFilter = VK_FILTER_NEAREST;
		VkSamplerMipmapMode     m_mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		VkSamplerAddressMode    m_addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode    m_addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkSamplerAddressMode    m_addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		float                   m_mipLodBias = 0.0f;
		VkBool32                m_anisotropyEnable = VK_FALSE;
		float                   m_maxAnisotropy = 0.0f;
		VkBool32                m_compareEnable = VK_FALSE;
		VkCompareOp             m_compareOp = VK_COMPARE_OP_NEVER;
		float                   m_minLod = 0.0f;
		float                   m_maxLod = 0.0f;
		VkBorderColor           m_borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		VkBool32                m_unnormalizedCoordinates = VK_FALSE;
	};

	struct ImageDescriptorInfo {
		VkDescriptorSet                     m_targetDescriptorSet;
		uint32_t                            m_targetDescriptorBinding;
		uint32_t                            m_targetArrayElement;
		VkDescriptorType                    m_targetDescriptorType;
		std::vector<VkDescriptorImageInfo>  m_imageInfos;
	};

	struct BufferDescriptorInfo {
		VkDescriptorSet                     m_targetDescriptorSet;
		uint32_t                            m_targetDescriptorBinding;
		uint32_t                            m_targetArrayElement;
		VkDescriptorType                    m_targetDescriptorType;
		std::vector<VkDescriptorBufferInfo> m_bufferInfos;
	};

	struct TexelBufferDescriptorInfo {
		VkDescriptorSet                     m_targetDescriptorSet;
		uint32_t                            m_targetDescriptorBinding;
		uint32_t                            m_targetArrayElement;
		VkDescriptorType                    m_targetDescriptorType;
		std::vector<VkBufferView>           m_texelBufferViews;
	};

	struct CopyDescriptorInfo {
		VkDescriptorSet     m_targetDescriptorSet;
		uint32_t            m_targetDescriptorBinding;
		uint32_t            m_targetArrayElement;
		VkDescriptorSet     m_sourceDescriptorSet;
		uint32_t            m_sourceDescriptorBinding;
		uint32_t            m_sourceArrayElement;
		uint32_t            m_descriptorCount;
	};

	
	struct SubpassParameters
	{
		VkPipelineBindPoint                  m_pipelineType;
		std::vector<VkAttachmentReference>   m_inputAttachments;
		std::vector<VkAttachmentReference>   m_colorAttachments;
		std::vector<VkAttachmentReference>   m_resolveAttachments;
		VkAttachmentReference const*		 m_depthStencilAttachment;
		std::vector<uint32_t>                m_preserveAttachments;
	};

	struct WaitSemaphoreInfo
	{
		VkSemaphore           m_semaphore;
		VkPipelineStageFlags  m_waitingStage;
	};

	struct PresentInfo {
		VkSwapchainKHR  m_swapchain;
		uint32_t        m_imageIndex;
	};
}

#endif
