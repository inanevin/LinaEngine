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

#include "Graphics/PipelineObjects/Swapchain.hpp"
#include "Graphics/Utility/Vulkan/VkBootstrap.h"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/PipelineObjects/Semaphore.hpp"
#include "Graphics/PipelineObjects/Fence.hpp"
#include "Core/CommonEngine.hpp"
#include "Log/Log.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    void Swapchain::Create()
    {
        LINA_ASSERT(_ptr == nullptr, "[Swapchain] -> Can not re-create swapchain before it's destroyed!");

        if (size.x == 0 || size.y == 0)
        {
            LINA_ERR("[Swapchain] -> Could not create swapchain, width or height is 0!");
            return;
        }

        vkb::SwapchainBuilder swapchainBuilder{Backend::Get()->GetGPU(), Backend::Get()->GetDevice(), surface};
        swapchainBuilder = swapchainBuilder
                               //.use_default_format_selection()
                               .set_desired_present_mode(GetPresentMode(presentMode))
                               .set_desired_extent(size.x, size.y);

        VkFormat        vkformat     = GetFormat(format);
        VkColorSpaceKHR vkcolorspace = GetColorSpace(colorSpace);
        swapchainBuilder             = swapchainBuilder.set_desired_format({vkformat, vkcolorspace});

        vkb::Swapchain vkbSwapchain = swapchainBuilder.build().value();
        _ptr                        = vkbSwapchain.swapchain;
        _format                     = vkbSwapchain.image_format;

        std::vector<VkImage>     imgs  = vkbSwapchain.get_images().value();
        std::vector<VkImageView> views = vkbSwapchain.get_image_views().value();

        size.x                    = vkbSwapchain.extent.width;
        size.y                    = vkbSwapchain.extent.height;
        RuntimeInfo::s_screenSize = size;

        for (VkImage img : imgs)
            _images.push_back(img);

        for (VkImageView view : views)
            _imageViews.push_back(view);
    }

    void Swapchain::Destroy()
    {
        if (_ptr != nullptr)
        {
            auto*       device    = Backend::Get()->GetDevice();
            const auto* allocator = Backend::Get()->GetAllocator();

            // Destroy existing swapchain.
            vkDestroySwapchainKHR(device, _ptr, allocator);

            for (auto view : _imageViews)
                vkDestroyImageView(device, view, allocator);

            _images.clear();
            _imageViews.clear();
            _ptr = nullptr;
        }
    }

    uint32 Swapchain::AcquireNextImage(double timeoutSeconds, const Semaphore& semaphore, VulkanResult& res) const
    {
        uint32   index;
        uint64   timeout = static_cast<uint64>(timeoutSeconds * 1000000000);
        VkResult result  = vkAcquireNextImageKHR(Backend::Get()->GetDevice(), _ptr, timeout, semaphore._ptr, nullptr, &index);
        res              = GetResult(static_cast<int32>(result));
        return index;
    }
    uint32 Swapchain::AcquireNextImage(double timeoutSeconds, const Semaphore& semaphore, const Fence& fence) const
    {
        uint32   index;
        uint64   timeout = static_cast<uint64>(timeoutSeconds * 1000000000);
        VkResult result  = vkAcquireNextImageKHR(Backend::Get()->GetDevice(), _ptr, timeout, semaphore._ptr, fence._ptr, &index);
        LINA_ASSERT(result == VK_SUCCESS, "[Swapchain] -> Could not acquire next image!");
        return index;
    }
    uint32 Swapchain::AcquireNextImage(double timeoutSeconds, const Fence& fence) const
    {
        uint32   index;
        uint64   timeout = static_cast<uint64>(timeoutSeconds * 1000000000);
        VkResult result  = vkAcquireNextImageKHR(Backend::Get()->GetDevice(), _ptr, timeout, nullptr, nullptr, &index);
        LINA_ASSERT(result == VK_SUCCESS, "[Swapchain] -> Could not acquire next image!");
        return index;
    }
} // namespace Lina::Graphics
