/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Graphics/Platform/Vulkan/Objects/Swapchain.hpp"
#include "Graphics/Platform/Vulkan/Utility/VkBootstrap.h"
#include "Graphics/Platform/Vulkan/Core/GfxBackend.hpp"
#include "Graphics/Platform/Vulkan/Objects/Semaphore.hpp"
#include "Graphics/Platform/Vulkan/Objects/Fence.hpp"
#include "Log/Log.hpp"
#include "Graphics/Platform/Vulkan/Utility/VulkanUtility.hpp"
#include <vulkan/vulkan.h>

namespace Lina
{
	void Swapchain::Create(StringID sid)
	{
		swapchainID = sid;

		if (size.x == 0 || size.y == 0)
		{
			LINA_ERR("[Swapchain] -> Could not create swapchain, width or height is 0!");
			return;
		}

		vkb::SwapchainBuilder swapchainBuilder{gpu, device, surface};
		swapchainBuilder = swapchainBuilder
							   //.use_default_format_selection()
							   .set_desired_present_mode(GetPresentMode(presentMode))
							   .set_desired_extent(size.x, size.y);

		VkFormat		vkformat	 = GetFormat(format);
		VkColorSpaceKHR vkcolorspace = GetColorSpace(colorSpace);
		swapchainBuilder			 = swapchainBuilder.set_desired_format({vkformat, vkcolorspace});

		vkb::Swapchain vkbSwapchain = swapchainBuilder.build().value();
		_ptr						= vkbSwapchain.swapchain;
		_format						= vkbSwapchain.image_format;

		std::vector<VkImage>	 imgs  = vkbSwapchain.get_images().value();
		std::vector<VkImageView> views = vkbSwapchain.get_image_views().value();

		size.x = vkbSwapchain.extent.width;
		size.y = vkbSwapchain.extent.height;

		for (VkImage img : imgs)
		{
			_images.push_back(img);

			Image depthImg;
			VulkanUtility::CreateDefaultPassImageDepth(depthImg, size.x, size.y);
			_depthImages.push_back(depthImg);
		}

		for (VkImageView view : views)
			_imageViews.push_back(view);

		if (!_semaphoresInited)
		{
			_semaphoresInited = true;

			for (auto& s : _submitSemaphores)
			{
				s.device	   = device;
				s.allocationCb = allocationCb;
				s.Create();
			}
			for (auto& s : _presentSemaphores)
			{
				s.device	   = device;
				s.allocationCb = allocationCb;
				s.Create();
			}
		}
	}

	void Swapchain::RecreateFromOld(StringID sid)
	{
		swapchainID = sid;

		// Query surface capabilities and format
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceCapabilities);

		if (surfaceCapabilities.currentExtent.width == 0 || surfaceCapabilities.currentExtent.height == 0)
			return;

		if (size.x == 0 || size.y == 0)
		{
			LINA_ERR("[Swapchain] -> Could not create swapchain, width or height is 0!");
			return;
		}
		auto oldSwapchain = _ptr;

		vkb::SwapchainBuilder swapchainBuilder{gpu, device, surface};
		swapchainBuilder = swapchainBuilder.set_old_swapchain(oldSwapchain).set_desired_present_mode(GetPresentMode(presentMode)).set_desired_extent(size.x, size.y);

		VkFormat		vkformat	 = GetFormat(format);
		VkColorSpaceKHR vkcolorspace = GetColorSpace(colorSpace);
		swapchainBuilder			 = swapchainBuilder.set_desired_format({vkformat, vkcolorspace});

		vkb::Swapchain vkbSwapchain = swapchainBuilder.build().value();
		_ptr						= vkbSwapchain.swapchain;
		_format						= vkbSwapchain.image_format;

		std::vector<VkImage>	 imgs  = vkbSwapchain.get_images().value();
		std::vector<VkImageView> views = vkbSwapchain.get_image_views().value();

		size.x = vkbSwapchain.extent.width;
		size.y = vkbSwapchain.extent.height;

		if (oldSwapchain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

		for (auto view : _imageViews)
			vkDestroyImageView(device, view, allocationCb);

		for (auto t : _depthImages)
			t.Destroy();

		_images.clear();
		_imageViews.clear();
		_depthImages.clear();

		for (VkImage img : imgs)
		{
			_images.push_back(img);

			Image depthImg;
			VulkanUtility::CreateDefaultPassImageDepth(depthImg, size.x, size.y);
			_depthImages.push_back(depthImg);
		}

		for (VkImageView view : views)
			_imageViews.push_back(view);
	}

	void Swapchain::Destroy()
	{
		if (_ptr != nullptr)
		{
			// Destroy existing swapchain.
			vkDestroySwapchainKHR(device, _ptr, allocationCb);

			for (auto view : _imageViews)
				vkDestroyImageView(device, view, allocationCb);

			_images.clear();
			_imageViews.clear();
			_ptr = nullptr;

			for (auto t : _depthImages)
				t.Destroy();

			_depthImages.clear();
		}

		for (auto& s : _submitSemaphores)
			s.Destroy();

		for (auto& s : _presentSemaphores)
			s.Destroy();
	}

	uint32 Swapchain::AcquireNextImage(uint64 timeout, const Semaphore& semaphore, VulkanResult& res) const
	{
		// uint64	 timeout = static_cast<uint64>(timeoutSeconds * 1000000000);

		uint32	 index;
		VkResult result = vkAcquireNextImageKHR(device, _ptr, timeout, semaphore._ptr, nullptr, &index);
		res				= GetResult(static_cast<int32>(result));
		return index;
	}
	uint32 Swapchain::AcquireNextImage(uint64 timeout, const Semaphore& semaphore, VulkanResult& res, const Fence& fence) const
	{
		uint32	 index;
		VkResult result = vkAcquireNextImageKHR(device, _ptr, timeout, semaphore._ptr, fence._ptr, &index);
		res				= GetResult(static_cast<int32>(result));
		return index;
	}
	uint32 Swapchain::AcquireNextImage(uint64 timeout, const Fence& fence) const
	{
		uint32	 index;
		VkResult result = vkAcquireNextImageKHR(device, _ptr, timeout, nullptr, nullptr, &index);
		LINA_ASSERT(result == VK_SUCCESS, "[Swapchain] -> Could not acquire next image!");
		return index;
	}
} // namespace Lina
