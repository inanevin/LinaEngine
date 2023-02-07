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

#ifndef GfxBackend_HPP
#define GfxBackend_HPP

#include "Graphics/Platform/Vulkan/Objects/Queue.hpp"
#include "Data/CommonData.hpp"
#include "Core/Common.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Event/IEventListener.hpp"

struct VkDevice_T;
struct VkPhysicalDevice_T;
struct VkSurfaceKHR_T;
struct VkAllocationCallbacks;
struct VkDevice_T;
struct VkDebugUtilsMessengerEXT_T;
struct VkInstance_T;
struct VmaAllocator_T;
struct VkImageView_T;

namespace Lina
{
	struct QueueFamily
	{
		uint32 flags = 0;
		uint32 count = 0;
	};

	struct SystemInitializationInfo;
	class Window;
	class ISystem;

	class GfxBackend : IEventListener
	{
	public:
		GfxBackend(ISystem* system);
		virtual ~GfxBackend();

		//************************ ENGINE API ************************//
		void		 Initialize(const SystemInitializationInfo& initInfo, Window* mainWindow);
		void		 Shutdown();
		void		 CreateSwapchain(StringID sid, void* windowPtr, void* instancePtr, const Vector2i& pos, const Vector2i& size);
		void		 DestroySwapchain(StringID sid);
		virtual void OnSystemEvent(ESystemEvent type, const Event& ev) override;

		virtual Bitmask32 GetSystemEventMask()
		{
			return EVS_VsyncMode;
		}

		//************************ ENGINE API ************************//

		void		WaitIdle();
		PresentMode VsyncToPresentMode(VsyncMode mode);
		inline bool SupportsAsyncTransferQueue()
		{
			return m_supportsAsyncTransferQueue;
		}

		inline bool SupportsAsyncComputeQueue()
		{
			return m_supportsAsyncComputeQueue;
		}

		inline const RQueue& GetGraphicsQueue()
		{
			return m_graphicsQueue;
		}

		inline const RQueue& GetTransferQueue()
		{
			return m_transferQueue;
		}

		inline const RQueue& GetComputeQueue()
		{
			return m_computeQueue;
		}

		inline VkDevice_T* GetDevice()
		{
			return m_device;
		}

		inline VkAllocationCallbacks* GetAllocationCb()
		{
			return m_allocator;
		}

		inline VmaAllocator_T* GetVMA()
		{
			return m_vmaAllocator;
		}

	private:
		ISystem*						   m_system			= nullptr;
		VkInstance_T*					   m_vkInstance		= nullptr;
		VkDebugUtilsMessengerEXT_T*		   m_debugMessenger = nullptr;
		VkAllocationCallbacks*			   m_allocator		= nullptr;
		VkDevice_T*						   m_device			= nullptr;
		VkPhysicalDevice_T*				   m_gpu			= nullptr;
		VmaAllocator_T*					   m_vmaAllocator	= nullptr;
		Vector<QueueFamily>				   m_queueFamilies;
		uint64							   m_minUniformBufferOffsetAlignment = 0;
		Pair<uint32, uint32>			   m_graphicsQueueIndices;
		Pair<uint32, uint32>			   m_transferQueueIndices;
		Pair<uint32, uint32>			   m_computeQueueIndices;
		RQueue							   m_graphicsQueue;
		RQueue							   m_transferQueue;
		RQueue							   m_computeQueue;
		HashMap<StringID, Swapchain*>	   m_swapchains;
		HashMap<StringID, VkSurfaceKHR_T*> m_surfaces;
		Vector<Format>					   m_supportedGameTextureFormats;
		PresentMode						   m_currentPresentMode			= PresentMode::Mailbox;
		Format							   m_defaultFormat4Ch			= Format::UNDEFINED;
		Format							   m_defaultFormat2Ch			= Format::UNDEFINED;
		Format							   m_defaultFormat1Ch			= Format::UNDEFINED;
		bool							   m_supportsAsyncTransferQueue = false;
		bool							   m_supportsAsyncComputeQueue	= false;
	};
} // namespace Lina

#endif