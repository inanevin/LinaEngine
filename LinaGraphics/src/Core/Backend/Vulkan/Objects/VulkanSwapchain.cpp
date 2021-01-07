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

#include "Core/Backend/Vulkan/Objects/VulkanSwapchain.hpp"
#include "Core/Log.hpp"
#include "Core/Backend/Vulkan/Utility/VulkanFunctions.hpp"
#include "Math/Math.hpp"


namespace Lina::Graphics
{
	bool VulkanSwapchain::Create(const SwapchainData& data)
	{
		// Select desired presentation mode.
		VkPresentModeKHR selectedPresentMode;
		if (!SelectDesiredPresentationMode(data, selectedPresentMode))
			return VK_NULL_HANDLE;

		// Retrieve surface capabilities.
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VkResult surfaceResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(data.vulkanData->m_physicalDevice, data.vulkanData->m_surface, &surfaceCapabilities);
		if (VK_SUCCESS != surfaceResult)
		{
			LINA_ERR("[Vulkan Swapchain] -> Could not get the capabilities of a presentation surface.");
			return VK_NULL_HANDLE;
		}

		// Find out number of images for the swap chain.
		uint32_t imagesCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount > 0 && surfaceCapabilities.maxImageCount > imagesCount)
			imagesCount = surfaceCapabilities.maxImageCount;

		// Determine the size of images for the swapchain, some OS won't let you have custom size.
		VkExtent2D imagesSize;
		if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
		{
			imagesSize.width = data.windowProps->m_width;
			imagesSize.height = data.windowProps->m_height;
			imagesSize.width = Math::Clamp(imagesSize.height, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			imagesSize.height = Math::Clamp(imagesSize.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}
		else
			imagesSize = surfaceCapabilities.currentExtent;

		// Check desired image usages.
		VkImageUsageFlags imageUsage;
		imageUsage = data.imageUsageFlags & surfaceCapabilities.supportedUsageFlags;
		if (data.imageUsageFlags != imageUsage)
		{
			LINA_ERR("[Vulkan Swapchain] -> Desired swap chain usages are not supported!");
			return false;
		}

		// Check transforms.
		VkSurfaceTransformFlagBitsKHR surfaceTransform;
		if (surfaceCapabilities.supportedTransforms & data.transformFlags)
			surfaceTransform = data.transformFlags;
		else
			surfaceTransform = surfaceCapabilities.currentTransform;

		// Determine image format & colorspaces.
		VkFormat selectedFormat;
		VkColorSpaceKHR selectedColorspace;
		if (!SelectFormatOfSwapchainImages(data, selectedFormat, selectedColorspace))
			return false;

		VkSwapchainKHR oldSwapchain = m_handle;

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                  sType
		nullptr,                                      // const void                     * pNext
		0,                                            // VkSwapchainCreateFlagsKHR        flags
		data.vulkanData->m_surface,                     // VkSurfaceKHR                     surface
		imagesCount,                                  // uint32_t                         minImageCount
		selectedFormat,								// VkFormat                         imageFormat
		selectedColorspace,							// VkColorSpaceKHR                  imageColorSpace
		imagesSize,                                   // VkExtent2D                       imageExtent
		1,                                            // uint32_t                         imageArrayLayers
		imageUsage,                                  // VkImageUsageFlags                imageUsage
		VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                    imageSharingMode
		0,                                            // uint32_t                         queueFamilyIndexCount
		nullptr,                                      // const uint32_t                 * pQueueFamilyIndices
		surfaceTransform,                            // VkSurfaceTransformFlagBitsKHR    preTransform
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR      compositeAlpha
		selectedPresentMode,                                 // VkPresentModeKHR                 presentMode
		VK_TRUE,                                      // VkBool32                         clipped
		oldSwapchain                                 // VkSwapchainKHR                   oldSwapchain
		};

		VkResult result = vkCreateSwapchainKHR(data.vulkanData->m_logicalDevice, &swapchainCreateInfo, nullptr, &m_handle);
		if ((VK_SUCCESS != result) || (VK_NULL_HANDLE == m_handle))
		{
			LINA_ERR("[Vulkan Swapchain] -> Could not create a swapchain.");
			return false;
		}

		if (VK_NULL_HANDLE != oldSwapchain)
		{
			vkDestroySwapchainKHR(data.vulkanData->m_logicalDevice, oldSwapchain, nullptr);
			oldSwapchain = VK_NULL_HANDLE;
		}
		m_imagesSize = imagesSize;
		m_imageFormat = selectedFormat;
		m_images.clear();

		uint32_t createdImagesCount;
		result = vkGetSwapchainImagesKHR(data.vulkanData->m_logicalDevice, m_handle, &createdImagesCount, nullptr);
		if (result != VK_SUCCESS || createdImagesCount == 0)
		{
			LINA_ERR("[Vulkan Swapchain] -> Could not retrieve swapchain images.");
			return false;
		}

		m_images.resize(createdImagesCount);

		result = vkGetSwapchainImagesKHR(data.vulkanData->m_logicalDevice, m_handle, &imagesCount, &m_images[0]);
		if (result != VK_SUCCESS || createdImagesCount == 0)
		{
			LINA_ERR("[Vulkan Swapchain] -> Could not retrieve swapchain images.");
			return false;
		}

		CreateImageViews(data.vulkanData, createdImagesCount);

		LINA_TRACE("[Vulkan Swapchain] -> Successfuly a created swapchain");
		return true;
	}

	uint32_t VulkanSwapchain::AcquireNextImage(VkDevice logicalDevice, VkSemaphore semaphore, VkFence fence)
	{
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(logicalDevice, m_handle, 2000000000, semaphore, fence, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			LINA_CRITICAL("[Vulkan Swapchain] -> Can not get swapchain image, swapchain is out of date!");
			return VK_NULL_HANDLE;
		}
		else if (result == VK_SUBOPTIMAL_KHR)
			LINA_WARN("[Vulkan Swapchain] -> Swapchain image is suboptimal.");

		return imageIndex;
	}

	void VulkanSwapchain::Destroy(VkDevice logicalDevice)
	{
		for (auto& view : m_imageViews)
		{
			if (view != VK_NULL_HANDLE)
				vkDestroyImageView(logicalDevice, view, nullptr);
		}
		if (m_handle)
		{
			vkDestroySwapchainKHR(logicalDevice, m_handle, nullptr);
			m_handle = VK_NULL_HANDLE;
		}
		m_images.clear();
		m_imageViews.clear();

	}

	bool VulkanSwapchain::CreateImageViews(VulkanData* data, uint32_t createdImagesCount)
	{
		m_imageViews.resize(createdImagesCount);
		VkComponentMapping  components;
		VkImageSubresourceRange    subresourceRange;
		components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		for (int i = 0; i < m_imageViews.size(); ++i)
		{

			VkImageViewCreateInfo imgViewCreateInfo
			{
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				nullptr,
				0,
				m_images[i],
				VK_IMAGE_VIEW_TYPE_2D,
				m_imageFormat,
				components,
				subresourceRange
			};
			VkResult result = vkCreateImageView(data->m_logicalDevice, &imgViewCreateInfo, nullptr, &m_imageViews[i]);
			if (result != VK_SUCCESS)
			{
				LINA_ERR("[Vulkan Swapchain] -> Could not create image views for the created images.");
				return false;
			}
		}
	}

	bool VulkanSwapchain::SelectDesiredPresentationMode(const SwapchainData& data, VkPresentModeKHR& outSelectedMode)
	{
		// Enumerate supported present modes
		uint32_t presentModesCount = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(data.vulkanData->m_physicalDevice, data.vulkanData->m_surface, &presentModesCount, nullptr);
		if ((VK_SUCCESS != result) || (0 == presentModesCount))
		{
			LINA_ERR("[Vulkan Handler] -> Could not get the number of supported present modes.");
			return false;
		}

		std::vector<VkPresentModeKHR> presentModes(presentModesCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(data.vulkanData->m_physicalDevice, data.vulkanData->m_surface, &presentModesCount, presentModes.data());
		if ((VK_SUCCESS != result) || (0 == presentModesCount))
		{
			LINA_ERR("[Vulkan Handler] -> Could not enumerate present modes.");
			return false;
		}

		// Select present mode
		for (auto& mode : presentModes)
		{
			if (mode == data.desiredPresentMode)
			{
				outSelectedMode = data.desiredPresentMode;
				return true;
			}
		}

		LINA_WARN("[Vulkan Handler] -> Desired present mode is not supported. Selecting default FIFO mode.");
		for (auto& mode : presentModes)
		{
			if (mode == VK_PRESENT_MODE_FIFO_KHR)
			{
				outSelectedMode = VK_PRESENT_MODE_FIFO_KHR;
				return true;
			}
		}

		LINA_WARN("[Vulkan Handler] -> VK_PRESENT_MODE_FIFO_KHR is not supported, how do you even?");
		return false;
	}

	bool VulkanSwapchain::SelectFormatOfSwapchainImages(const SwapchainData& data, VkFormat& outSelectedImageFormat, VkColorSpaceKHR& outSelectedColorSpace)
	{
		// Enumerate supported formats
		uint32_t formats_count = 0;
		VkResult result = VK_SUCCESS;

		result = vkGetPhysicalDeviceSurfaceFormatsKHR(data.vulkanData->m_physicalDevice, data.vulkanData->m_surface, &formats_count, nullptr);
		if ((VK_SUCCESS != result) || (0 == formats_count))
		{
			LINA_TRACE("[Vulkan Swapchain] -> Could not get the number of supported surface formats.");
			return false;
		}

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formats_count);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(data.vulkanData->m_physicalDevice, data.vulkanData->m_surface, &formats_count, &surfaceFormats[0]);
		if ((VK_SUCCESS != result) || (0 == formats_count))
		{
			LINA_TRACE("[Vulkan Swapchain] -> Could not enumerate supported surface formats.");
			return false;
		}

		// Select surface format
		if ((1 == surfaceFormats.size()) && (VK_FORMAT_UNDEFINED == surfaceFormats[0].format))
		{
			outSelectedImageFormat = data.desiredSurfaceFormat.format;
			outSelectedColorSpace = data.desiredSurfaceFormat.colorSpace;
			return true;
		}

		for (auto& sformat : surfaceFormats)
		{
			if ((data.desiredSurfaceFormat.format == sformat.format) && (data.desiredSurfaceFormat.colorSpace == sformat.colorSpace))
			{
				outSelectedImageFormat = data.desiredSurfaceFormat.format;
				outSelectedColorSpace = data.desiredSurfaceFormat.colorSpace;
				return true;
			}
		}

		for (auto& sformat : surfaceFormats)
		{
			if ((data.desiredSurfaceFormat.format == sformat.format))
			{
				outSelectedImageFormat = data.desiredSurfaceFormat.format;
				outSelectedColorSpace = sformat.colorSpace;
				LINA_TRACE("[Vulkan Swapchain] -> Desired combination of format and colorspace is not supported. Selecting other colorspace.");
				return true;
			}
		}

		outSelectedImageFormat = surfaceFormats[0].format;
		outSelectedColorSpace = surfaceFormats[0].colorSpace;
		LINA_TRACE("[Vulkan Swapchain] -> Desired format is not supported. Selecting available format - colorspace combination.");
		return true;
	}
}