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

#include "Data/Vector.hpp"
#include "Math/Vector.hpp"
#include "Utility/StringId.hpp"
#include "Graphics/Core/GraphicsCommon.hpp"

struct VkImage_T;
struct VkImageView_T;
struct VkSwapchainKHR_T;
struct VkSurfaceKHR_T;

namespace Lina::Graphics
{
    class Semaphore;
    class Fence;
    class Swapchain
    {
    public:
        void   Create(StringID sid);
        void   Destroy();
        uint32 AcquireNextImage(double timeoutSeconds, const Semaphore& semaphore, VulkanResult& result) const;
        uint32 AcquireNextImage(double timeoutSeconds, const Semaphore& semaphore, const Fence& fence) const;
        uint32 AcquireNextImage(double timeoutSeconds, const Fence& fence) const;

        // Desired
        Vector2i        size        = Vector2i();
        Vector2         pos         = Vector2::Zero;
        Format          format      = Format::B8G8R8A8_SRGB;
        ColorSpace      colorSpace  = ColorSpace::SRGB_NONLINEAR;
        PresentMode     presentMode = PresentMode::Immediate;
        VkSurfaceKHR_T* surface     = nullptr;
        StringID        swapchainID = 0;

        // Runtime
        VkSwapchainKHR_T*      _ptr          = nullptr;
        VkSwapchainKHR_T*      _oldSwapchain = nullptr;
        Vector<VkImage_T*>     _images;
        Vector<VkImageView_T*> _imageViews;
        VkFormat               _format;
        void*                  _windowHandle = nullptr;
    };
} // namespace Lina::Graphics

#endif
