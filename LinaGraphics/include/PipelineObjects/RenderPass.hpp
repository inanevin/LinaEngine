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

#ifndef RenderPass_HPP
#define RenderPass_HPP

#include "Core/GraphicsCommon.hpp"
#include "Data/HashMap.hpp"
#include "PipelineObjects/Attachment.hpp"

struct VkRenderPass_T;

namespace Lina::Graphics
{
    class Framebuffer;
    class CommandBuffer;

    class SubPass
    {
    public:
        SubPass Create();
        SubPass AddColorAttachmentRef(uint32 index, ImageLayout layout);
        SubPass SetDepthStencilAttachmentRef(uint32 index, ImageLayout layout);

        // Description
        PipelineBindPoint bindPoint = PipelineBindPoint::Graphics;

        // Runtime
        HashMap<uint32, ImageLayout> _colorAttachmentRefs;
        uint32                       _depthStencilAttachmentIndex  = 0;
        ImageLayout                  _depthStencilAttachmentLayout = ImageLayout::DepthStencilOptimal;
        bool                         _depthStencilAttachmentSet    = false;
    };

    class RenderPass
    {
    public:
        RenderPass Create();
        RenderPass AddSubpass(SubPass sp);
        RenderPass AddAttachment(Attachment att);
        void       Begin(const ClearValue& clear, const Framebuffer& fb, const CommandBuffer& cmd);
        void       End(const CommandBuffer& cmd);

        // Description
        Vector<Attachment> attachments;
        Vector<SubPass>    subpasses;

        // Runtime
        VkRenderPass_T* _ptr = nullptr;
    };
} // namespace Lina::Graphics

#endif
