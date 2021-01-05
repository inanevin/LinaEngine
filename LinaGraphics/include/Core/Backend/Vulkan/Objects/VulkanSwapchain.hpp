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
Class: VulkanSwapchain

Wrapper for VkSwapChainKHR

Timestamp: 12/27/2020 8:09:56 PM
*/

#pragma once

#ifndef VulkanSwapchain_HPP
#define VulkanSwapchain_HPP

// Headers here.
#include "Core/Common.hpp"
#include "Core/Backend/Vulkan/VulkanCommon.hpp"

namespace Lina::Graphics
{
	struct SwapchainData
	{
		WindowProperties* windowProps = nullptr;
		VulkanData* vulkanData = nullptr;
		VkPresentModeKHR desiredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		VkSurfaceTransformFlagBitsKHR transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		VkSurfaceFormatKHR desiredSurfaceFormat = { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		VkFormat imageFormat;
		VkColorSpaceKHR colorSpace;
	};

	class VulkanSwapchain
	{

	public:

	private:

		friend class RenderEngineVulkan;
		friend class VulkanHandler;
		friend class VulkanLogicalDevice;

		VulkanSwapchain() {};
		~VulkanSwapchain() {};

		bool SelectDesiredPresentationMode(const SwapchainData& data, VkPresentModeKHR& outSelectedMode);
		bool SelectFormatOfSwapchainImages(const SwapchainData& data, VkFormat& outSelectedImageFormat, VkColorSpaceKHR& outSelectedColorSpace);
		bool Create(const SwapchainData& data);
		VkImage GetImage(VkDevice logicalDevice, VkSemaphore semaphore, VkFence fence = VK_NULL_HANDLE);
		void Destroy(VkDevice logicalDevice);
		inline std::vector<VkImageView>& GetImageViews() { return m_imageViews; }

		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		VkSwapchainKHR m_handle = VK_NULL_HANDLE;
		VkExtent2D m_imagesSize;
		VkFormat m_imageFormat;
	};
}

#endif
