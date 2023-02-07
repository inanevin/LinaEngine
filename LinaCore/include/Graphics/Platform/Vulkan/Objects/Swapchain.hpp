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

#ifndef Swapchain_HPP
#define Swapchain_HPP

#include "Graphics/Data/RenderData.hpp"
#include "Data/Vector.hpp"
#include "Math/Vector.hpp"
#include "Semaphore.hpp"
#include "Image.hpp"
#include "Core/StringID.hpp"

struct VkImage_T;
struct VkImageView_T;
struct VkSwapchainKHR_T;
struct VkSurfaceKHR_T;
struct VkPhysicalDevice_T;
struct VkAllocationCallbacks;
struct VkDevice_T;

namespace Lina
{
	class Semaphore;
	class Fence;

	class Swapchain
	{
	public:
		void   Create(StringID sid);
		void   RecreateFromOld(StringID sid);
		void   Destroy();
		uint32 AcquireNextImage(double timeoutSeconds, const Semaphore& semaphore, VulkanResult& result) const;
		uint32 AcquireNextImage(double timeoutSeconds, const Semaphore& semaphore, const Fence& fence) const;
		uint32 AcquireNextImage(double timeoutSeconds, const Fence& fence) const;

		// Desired
		VkAllocationCallbacks* allocationCb = nullptr;
		VkDevice_T*			   device		= nullptr;
		VkPhysicalDevice_T*	   gpu			= nullptr;
		Vector2i			   size			= Vector2i();
		Vector2i			   pos			= Vector2i::Zero;
		Format				   format		= Format::B8G8R8A8_SRGB;
		ColorSpace			   colorSpace	= ColorSpace::SRGB_NONLINEAR;
		PresentMode			   presentMode	= PresentMode::Immediate;
		VkSurfaceKHR_T*		   surface		= nullptr;
		StringID			   swapchainID	= 0;

		// Runtime
		bool				   _semaphoresInited = false;
		Semaphore			   _submitSemaphores[FRAMES_IN_FLIGHT];
		Semaphore			   _presentSemaphores[FRAMES_IN_FLIGHT];
		VkSwapchainKHR_T*	   _ptr = nullptr;
		Vector<VkImage_T*>	   _images;
		Vector<VkImageView_T*> _imageViews;
		VkFormat			   _format;
		void*				   _windowHandle = nullptr;
		Vector<Image>		   _depthImages;
	};
} // namespace Lina

#endif
