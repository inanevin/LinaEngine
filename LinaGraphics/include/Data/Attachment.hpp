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

#ifndef Attachment_HPP
#define Attachment_HPP

#include "Core/GraphicsCommon.hpp"
#include "Core/SizeDefinitions.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    // Description of the image we'll be writing into w/ render commands.
    class Attachment
    {
    public:
        Attachment Create();

        // Description
        Format      format         = Format::B8G8R8A8_SRGB;
        LoadOp      loadOp         = LoadOp::Load;
        StoreOp     storeOp        = StoreOp::Store;
        LoadOp      stencilLoadOp  = LoadOp::DontCare;
        StoreOp     stencilStoreOp = StoreOp::DontCare;
        ImageLayout initialLayout  = ImageLayout::Undefined;
        ImageLayout finalLayout    = ImageLayout::PresentSurface;

        // Runtime
        VkAttachmentDescription _desc;
    };
} // namespace Lina::Graphics

#endif
