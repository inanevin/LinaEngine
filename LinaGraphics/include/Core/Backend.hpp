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
#include "Core/CommonApplication.hpp"
#include "Data/Swapchain.hpp"

struct VkDevice_T;
struct VkPhysicalDevice_T;
struct VkSurfaceKHR_T;
struct VkAllocationCallbacks;
struct VkDevice_T;
struct VkDebugUtilsMessengerEXT_T;
struct VkInstance_T;
struct VmaAllocator_T;

namespace Lina
{
    namespace Event
    {
        struct EWindowResized;
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
            return m_surface;
        }

        inline const VkAllocationCallbacks* GetAllocator()
        {
            return m_allocator;
        }

        inline const Swapchain& GetSwapchain()
        {
            return m_swapchain;
        }

        inline VmaAllocator_T* GetVMA()
        {
            return m_vmaAllocator;
        }

    private:
        friend class RenderEngine;

        Backend()  = default;
        ~Backend() = default;

        bool        Initialize(const ApplicationInfo& appInfo);
        void        Shutdown();
        uint32      GetQueueFamilyIndex(QueueFamilies family);
        void        OnWindowResized(const Event::EWindowResized& ev);
        void        OnVsyncModeChanged(const Event::EVsyncModeChanged& ev);
        PresentMode VsyncToPresentMode(VsyncMode mode);

    private:
        static Backend*             s_instance;
        ApplicationInfo             m_appInfo;
        VkInstance_T*               m_vkInstance     = nullptr;
        VkDebugUtilsMessengerEXT_T* m_debugMessenger = nullptr;
        VkAllocationCallbacks*      m_allocator      = nullptr;
        VkDevice_T*                 m_device         = nullptr;
        VkPhysicalDevice_T*         m_gpu            = nullptr;
        VkSurfaceKHR_T*             m_surface        = nullptr;
        VmaAllocator_T*             m_vmaAllocator   = nullptr;
        Swapchain                   m_swapchain;
        Vector<QueueFamily>         m_queueFamilies;
    };
} // namespace Lina::Graphics

#endif
