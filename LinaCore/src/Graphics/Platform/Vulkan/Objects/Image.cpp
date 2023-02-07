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

#include "Graphics/Platform/Vulkan/Objects/Image.hpp"
#include "Graphics/Platform/Vulkan/Utility/VulkanUtility.hpp"
#include "Log/Log.hpp"
#include "Data/DelegateQueue.hpp"
#include "Graphics/Platform/Vulkan/Utility/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace Lina
{
	void Image::Create(bool createImageView)
	{
		VkImageCreateInfo imgInfo = VulkanUtility::GetImageCreateInfo(format, imageUsageFlags, tiling, extent, initialLayout, sharingMode, mipLevels);

		VmaAllocationCreateInfo allocinfo = VmaAllocationCreateInfo{
			.usage		   = GetMemoryUsageFlags(memoryUsageFlags),
			.requiredFlags = VkMemoryPropertyFlags(GetMemoryPropertyFlags(memoryPropertyFlags)),
		};

		VkResult res = vmaCreateImage(vmaAllocator, &imgInfo, &allocinfo, &_allocatedImg.image, &_allocatedImg.allocation, nullptr);
		LINA_ASSERT(res == VK_SUCCESS, "[Image] -> Could not create image!");

		if (createImageView)
		{
			VkImageViewCreateInfo viewInfo = VulkanUtility::GetImageViewCreateInfo(_allocatedImg.image, format, subresRange);
			res							   = vkCreateImageView(device, &viewInfo, allocationCb, &_ptrImgView);
			LINA_ASSERT(res == VK_SUCCESS, "[Image] -> Could not create image view!");
		}

		VkImageView	  ptrImageView = _ptrImgView;
		VkImage		  ptrImg	   = _allocatedImg.image;
		VmaAllocation allocation   = _allocatedImg.allocation;

		if (deletionQueue)
		{
			VmaAllocator_T* all = vmaAllocator;
			VkDevice_T*		dv	= device;
			deletionQueue->Push(std::bind([ptrImageView, ptrImg, allocation, all, dv]() {
				vmaDestroyImage(all, ptrImg, allocation);
				if (ptrImageView)
					vkDestroyImageView(dv, ptrImageView, nullptr);
			}));
		}
	}

	void Image::Destroy()
	{
		if (_ptrImgView != nullptr)
			vkDestroyImageView(device, _ptrImgView, nullptr);

		if (_allocatedImg.allocation != nullptr)
			vmaDestroyImage(vmaAllocator, _allocatedImg.image, _allocatedImg.allocation);
	}
} // namespace Lina
