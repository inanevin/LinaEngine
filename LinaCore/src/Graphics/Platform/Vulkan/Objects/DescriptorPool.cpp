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

#include "Graphics/Platform/Vulkan/Objects/DescriptorPool.hpp"
#include "Graphics/Platform/Vulkan/Core/GfxBackend.hpp"
#include "Data/DelegateQueue.hpp"
#include "Data/Vector.hpp"
#include "Log/Log.hpp"
#include <vulkan/vulkan.h>

namespace Lina
{
	void DescriptorPool::Create()
	{
		Vector<VkDescriptorPoolSize> _sizes;

		for (auto& s : _poolSizes)
		{
			VkDescriptorPoolSize sizeInfo = VkDescriptorPoolSize{
				.type			 = GetDescriptorType(s.first),
				.descriptorCount = s.second,
			};

			_sizes.push_back(sizeInfo);
		}

		VkDescriptorPoolCreateInfo info = VkDescriptorPoolCreateInfo{
			.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags		   = GetDescriptorPoolCreateFlags(flags),
			.maxSets	   = maxSets,
			.poolSizeCount = static_cast<uint32>(_sizes.size()),
			.pPoolSizes	   = _sizes.data(),
		};

		VkResult res = vkCreateDescriptorPool(device, &info, allocationCb, &_ptr);
		LINA_ASSERT(res == VK_SUCCESS, "[Descriptor Pool] -> Could not create descriptor pool!");

		if (deletionQueue)
		{
			VkDescriptorPool_T*	   ptr = _ptr;
			VkDevice_T*			   dv  = device;
			VkAllocationCallbacks* cb  = allocationCb;
			deletionQueue->Push([ptr, dv, cb]() { vkDestroyDescriptorPool(dv, ptr, cb); });
		}
	}

	void DescriptorPool::Destroy()
	{
		if (_ptr != nullptr)
			vkDestroyDescriptorPool(device, _ptr, allocationCb);
	}

	DescriptorPool& DescriptorPool::AddPoolSize(DescriptorType type, uint32 count)
	{
		_poolSizes.push_back(linatl::make_pair(type, count));
		return *this;
	}
} // namespace Lina
