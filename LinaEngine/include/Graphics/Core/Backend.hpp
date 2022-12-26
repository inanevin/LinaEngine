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

#ifndef Backend_HPP
#define Backend_HPP

#include "Data/Vector.hpp"
#include "Data/DataCommon.hpp"
#include "Core/CommonApplication.hpp"
#include "Graphics/PipelineObjects/RQueue.hpp"
#include "Graphics/PipelineObjects/Swapchain.hpp"

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
    namespace Event
    {
        struct EVsyncModeChanged;
    } // namespace Event
} // namespace Lina
namespace Lina::Graphics
{
    struct QueueFamily
    {
        uint32 flags = 0;
        uint32 count = 0;
    };

    class Backend
    {
    public:
        Swapchain* CreateAdditionalSwapchain(StringID sid, void* windowPtr, int width, int height);
        void       DestroyAdditionalSwapchain(StringID sid);

        static Backend* Get()
        {
            return s_instance;
        }

        inline VkDevice_T* GetDevice()
        {
            return m_device;
        }

        inline VkPhysicalDevice_T* GetGPU()
        {
            return m_gpu;
        }

        inline VkSurfaceKHR_T* GetSurface()
        {
            return m_mainSurface;
        }

        inline const VkAllocationCallbacks* GetAllocator()
        {
            return m_allocator;
        }

        inline VmaAllocator_T* GetVMA()
        {
            return m_vmaAllocator;
        }

        inline const Swapchain& GetMainSwapchain()
        {
            return m_mainSwapchain;
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

        inline uint64 GetMinUniformBufferOffsetAlignment()
        {
            return m_minUniformBufferOffsetAlignment;
        }

        inline bool SupportsAsyncTransferQueue()
        {
            return m_supportsAsyncTransferQueue;
        }

        inline bool SupportsAsyncComputeQueue()
        {
            return m_supportsAsyncComputeQueue;
        }

        void WaitIdle();

    private:
        friend class RenderEngine;

        Backend()  = default;
        ~Backend() = default;

        bool        Initialize(const InitInfo& appInfo);
        void        Shutdown();
        void        OnVsyncModeChanged(const Event::EVsyncModeChanged& ev);
        PresentMode VsyncToPresentMode(VsyncMode mode);

    private:
        static Backend*              s_instance;
        VkInstance_T*                m_vkInstance     = nullptr;
        VkDebugUtilsMessengerEXT_T*  m_debugMessenger = nullptr;
        VkAllocationCallbacks*       m_allocator      = nullptr;
        VkDevice_T*                  m_device         = nullptr;
        VkPhysicalDevice_T*          m_gpu            = nullptr;
        VmaAllocator_T*              m_vmaAllocator   = nullptr;
        Vector<QueueFamily>          m_queueFamilies;
        uint64                       m_minUniformBufferOffsetAlignment = 0;
        Pair<uint32, uint32>         m_graphicsQueueIndices;
        Pair<uint32, uint32>         m_transferQueueIndices;
        Pair<uint32, uint32>         m_computeQueueIndices;
        RQueue                       m_graphicsQueue;
        RQueue                       m_transferQueue;
        RQueue                       m_computeQueue;
        bool                         m_supportsAsyncTransferQueue = false;
        bool                         m_supportsAsyncComputeQueue  = false;
        Swapchain                    m_mainSwapchain;
        VkSurfaceKHR_T*              m_mainSurface;
        PresentMode                  m_currentPresentMode;
        HashMap<StringID, Swapchain> m_additionalSwapchains;
    };
} // namespace Lina::Graphics

#endif
