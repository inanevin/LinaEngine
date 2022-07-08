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

#include "Data/Attachment.hpp"

namespace Lina::Graphics
{
    Attachment Attachment::Create()
    {
        _desc = VkAttachmentDescription{
            .format         = static_cast<VkFormat>(format),
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = static_cast<VkAttachmentLoadOp>(loadOp),
            .storeOp        = static_cast<VkAttachmentStoreOp>(storeOp),
            .stencilLoadOp  = static_cast<VkAttachmentLoadOp>(stencilLoadOp),
            .stencilStoreOp = static_cast<VkAttachmentStoreOp>(stencilStoreOp),
            .initialLayout  = static_cast<VkImageLayout>(initialLayout),
            .finalLayout    = static_cast<VkImageLayout>(finalLayout),
        };

        return *this;
    }
} // namespace Lina::Graphics
