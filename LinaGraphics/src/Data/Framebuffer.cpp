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

#include "Data/Framebuffer.hpp"
#include "Core/Backend.hpp"
#include "Data/RenderPass.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    Framebuffer Framebuffer::Create(VkImageView_T* imageView)
    {
        VkFramebufferCreateInfo info = VkFramebufferCreateInfo{
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext           = nullptr,
            .renderPass      = _renderPass,
            .attachmentCount = _attCount,
            .pAttachments    = &imageView,
            .width           = width,
            .height          = height,
            .layers          = layers,
        };

        VkResult result = vkCreateFramebuffer(Backend::Get()->GetDevice(), &info, Backend::Get()->GetAllocator(), &_ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Framebuffer] -> Could not create Vulkan framebuffer!");
        return *this;
    }
    Framebuffer Framebuffer::AttachRenderPass(const RenderPass& pass)
    {
        _renderPass = pass._ptr;
        _attCount   = static_cast<uint32>(pass.attachments.size());
        return *this;
    }
    void Framebuffer::Destroy()
    {
        if (_ptr != nullptr)
            vkDestroyFramebuffer(Backend::Get()->GetDevice(), _ptr, Backend::Get()->GetAllocator());
    }
} // namespace Lina::Graphics