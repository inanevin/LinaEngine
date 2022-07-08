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

#ifndef GraphicsCommon_HPP
#define GraphicsCommon_HPP
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    enum class Format
    {
        B8G8R8A8_SRGB = VK_FORMAT_B8G8R8A8_SRGB,
    };

    enum class ColorSpace
    {
        SRGB_NONLINEAR = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
    };

    enum class PresentMode
    {
        Immediate   = VK_PRESENT_MODE_IMMEDIATE_KHR,
        Mailbox     = VK_PRESENT_MODE_MAILBOX_KHR,
        FIFO        = VK_PRESENT_MODE_FIFO_KHR,
        FIFORelaxed = VK_PRESENT_MODE_FIFO_RELAXED_KHR,
    };

    enum class CommandBufferLevel
    {
        Primary   = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
    };

    enum class CommandPoolFlags
    {
        Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        Reset     = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        Protected = VK_COMMAND_POOL_CREATE_PROTECTED_BIT,
    };

    enum class QueueFamilies
    {
        Graphics      = VK_QUEUE_GRAPHICS_BIT,
        Compute       = VK_QUEUE_COMPUTE_BIT,
        Transfer      = VK_QUEUE_TRANSFER_BIT,
        SparseBinding = VK_QUEUE_SPARSE_BINDING_BIT,
        Protected     = VK_QUEUE_PROTECTED_BIT,
    };

    enum class ImageLayout
    {
        Undefined                   = VK_IMAGE_LAYOUT_UNDEFINED,
        General                     = VK_IMAGE_LAYOUT_GENERAL,
        ColorOptimal                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        DepthStencilOptimal         = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        DepthStencilReadOnlyOptimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
        PresentSurface              = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    enum class PipelineBindPoint
    {
        Graphics = VK_PIPELINE_BIND_POINT_GRAPHICS,
        Computer = VK_PIPELINE_BIND_POINT_COMPUTE,
    };

    enum class LoadOp
    {
        Load     = VK_ATTACHMENT_LOAD_OP_LOAD,
        Clear    = VK_ATTACHMENT_LOAD_OP_CLEAR,
        DontCare = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        None     = VK_ATTACHMENT_LOAD_OP_NONE_EXT,
    };

    enum class StoreOp
    {
        Store    = VK_ATTACHMENT_STORE_OP_STORE,
        DontCare = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        None     = VK_ATTACHMENT_STORE_OP_NONE,
        NoneKHR  = VK_ATTACHMENT_STORE_OP_NONE_KHR,
    };

    enum class FenceFlags
    {
        Signaled = VK_FENCE_CREATE_SIGNALED_BIT,
    };

} // namespace Lina::Graphics

#endif
