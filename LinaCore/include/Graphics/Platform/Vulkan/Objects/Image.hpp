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

#pragma once

#ifndef Image_HPP
#define Image_HPP

#include "Graphics/Platform/Vulkan/Utility/VulkanStructures.hpp"

struct VkImageView_T;
struct VkImage_T;
struct VmaAllocator_T;
struct VkDevice_T;
struct VkAllocationCallbacks;

namespace Lina
{
	class DelegateQueue;

	class Image
	{
	public:
		void Create(bool createImageView);
		void Destroy();

		// Description
		DelegateQueue*		   deletionQueue	   = nullptr;
		VmaAllocator_T*		   vmaAllocator		   = nullptr;
		VkDevice_T*			   device			   = nullptr;
		VkAllocationCallbacks* allocationCb		   = nullptr;
		Format				   format			   = Format::R32G32B32_SFLOAT;
		ImageTiling			   tiling			   = ImageTiling::Optimal;
		Extent3D			   extent			   = Extent3D();
		uint32				   imageUsageFlags	   = 0;
		MemoryUsageFlags	   memoryUsageFlags	   = MemoryUsageFlags::GpuOnly;
		MemoryPropertyFlags	   memoryPropertyFlags = MemoryPropertyFlags::None;
		ImageLayout			   initialLayout	   = ImageLayout::Undefined;
		SharingMode			   sharingMode		   = SharingMode::Exclusive;
		ImageSubresourceRange  subresRange;
		uint32				   mipLevels = 1;

		// Runtime
		AllocatedImage _allocatedImg;
		VkImageView_T* _ptrImgView = nullptr;
		bool		   _ready	   = false;
	};
} // namespace Lina

#endif
