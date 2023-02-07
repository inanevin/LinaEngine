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

#include "Graphics/Platform/Vulkan/Objects/Sampler.hpp"
#include "Log/Log.hpp"
#include "Data/DelegateQueue.hpp"
#include <vulkan/vulkan.h>

namespace Lina
{
	void Sampler::Create()
	{
		VkSamplerCreateInfo i = VkSamplerCreateInfo{
			.sType			  = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext			  = nullptr,
			.magFilter		  = GetFilter(samplerData.magFilter),
			.minFilter		  = GetFilter(samplerData.minFilter),
			.mipmapMode		  = GetMipmapMode(samplerData.mipmapMode),
			.addressModeU	  = GetSamplerAddressMode(samplerData.mode),
			.addressModeV	  = GetSamplerAddressMode(samplerData.mode),
			.addressModeW	  = GetSamplerAddressMode(samplerData.mode),
			.mipLodBias		  = samplerData.mipLodBias,
			.anisotropyEnable = samplerData.anisotropyEnabled,
			.maxAnisotropy	  = samplerData.anisotropy,
			.minLod			  = samplerData.minLod,
			.maxLod			  = samplerData.maxLod,
			.borderColor	  = GetBorderColor(samplerData.borderColor),
		};

		VkResult res = vkCreateSampler(device, &i, allocationCb, &_ptr);
		LINA_ASSERT(res == VK_SUCCESS, "[Sampler] -> Could not create sampler!");

		if (deletionQueue)
		{
			VkSampler_T*		   ptr = _ptr;
			VkAllocationCallbacks* all = allocationCb;
			VkDevice_T*			   dv  = device;
			deletionQueue->Push([ptr, all, dv]() { vkDestroySampler(dv, ptr, all); });
		}
	}

	void Sampler::Destroy()
	{
		if (_ptr == nullptr)
			return;

		vkDestroySampler(device, _ptr, allocationCb);
	}
} // namespace Lina
