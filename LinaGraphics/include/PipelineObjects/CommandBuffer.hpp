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

#ifndef CommandBuffer_HPP
#define CommandBuffer_HPP

#include "Core/GraphicsCommon.hpp"

struct VkCommandBuffer_T;
struct VkCommandPool_T;
struct VkBuffer_T;
struct VkPipelineLayout_T;

namespace Lina::Graphics
{
    class CommandBuffer
    {
    public:
        void Create(VkCommandPool_T* pool);

        void Reset(bool releaseResources = false);
        void Begin(uint32 flags);
        void BindVertexBuffers(uint32 firstBinding, uint32 bindingCount, VkBuffer_T* buffer, uint64* offset);
        void PushConstants(VkPipelineLayout_T* pipelineLayout, uint32 stageFlags, uint32 offset, uint32 size, void* constants);
        void Draw(uint32 vtxCount, uint32 instCount, uint32 firstVtx, uint32 firstInst);
        void End();

        // Description
        uint32             count = 0;
        CommandBufferLevel level = CommandBufferLevel::Primary;

        // Runtime
        VkCommandBuffer_T* _ptr = nullptr;
    };
} // namespace Lina::Graphics

#endif
