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

#include "Core/Swapchain.hpp"
#include "Utility/VkBootstrap.h"

namespace Lina::Graphics
{
    void Swapchain::Create(VkPhysicalDevice gpu, VkDevice device, VkSurfaceKHR surface)
    {
        LINA_ASSERT(_ptr == nullptr, "[Swapchain] -> Can not re-create swapchain before it's destroyed!");

        if (width == 0 || height == 0)
        {
            LINA_ERR("[Swapchain] -> Could not create swapchain, width or height is 0!");
            return;
        }

        vkb::SwapchainBuilder swapchainBuilder{gpu, device, surface};
        swapchainBuilder = swapchainBuilder
                               // use vsync present mode
                               .set_desired_present_mode(static_cast<VkPresentModeKHR>(presentMode))
                               .set_desired_extent(width, height);

        VkFormat        vkformat     = static_cast<VkFormat>(format);
        VkColorSpaceKHR vkcolorspace = static_cast<VkColorSpaceKHR>(colorSpace);
        swapchainBuilder             = swapchainBuilder.set_desired_format({vkformat, vkcolorspace});

        vkb::Swapchain vkbSwapchain = swapchainBuilder.build().value();
        _ptr                        = vkbSwapchain.swapchain;
        _format                     = vkbSwapchain.image_format;

        std::vector<VkImage>     imgs  = vkbSwapchain.get_images().value();
        std::vector<VkImageView> views = vkbSwapchain.get_image_views().value();

        for (VkImage img : imgs)
            _images.push_back(img);

        for (VkImageView view : views)
            _imageViews.push_back(view);

    }

    void Swapchain::Destroy(VkDevice device, const VkAllocationCallbacks* allocator)
    {
        if (_ptr != nullptr)
        {
            // Destroy existing swapchain.
            vkDestroySwapchainKHR(device, _ptr, allocator);

            for (auto view : _imageViews)
                vkDestroyImageView(device, view, allocator);

            _images.clear();
            _imageViews.clear();
            _ptr = nullptr;
        }
    }
} // namespace Lina::Graphics
