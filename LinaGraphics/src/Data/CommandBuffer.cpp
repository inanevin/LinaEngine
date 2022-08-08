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

#include "Data/CommandBuffer.hpp"
#include "Core/Backend.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    CommandBuffer CommandBuffer::Create(VkCommandPool_T* pool)
    {
        VkCommandBufferAllocateInfo cmdAllocInfo = VkCommandBufferAllocateInfo{
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = nullptr,
            .commandPool        = pool,
            .level              = GetCommandBufferLevel(level),
            .commandBufferCount = count,
        };

        VkResult result = vkAllocateCommandBuffers(Backend::Get()->GetDevice(), &cmdAllocInfo, &_ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Could not allocate command buffers!");
        return *this;
    }

    void CommandBuffer::Reset(bool releaseResources)
    {
        VkResult result = vkResetCommandBuffer(_ptr, releaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Resetting command buffer failed!");
    }

    void CommandBuffer::Begin(CommandBufferFlags flags)
    {
        VkCommandBufferBeginInfo beginInfo = VkCommandBufferBeginInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = nullptr,
            .flags            = static_cast<VkCommandBufferUsageFlags>(GetCommandBufferFlags(flags)),
            .pInheritanceInfo = nullptr,
        };

        VkResult result = vkBeginCommandBuffer(_ptr, &beginInfo);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Failed to begin!");
    }

    void CommandBuffer::Draw(uint32 vtxCount, uint32 instCount, uint32 firstVtx, uint32 firstInst)
    {
        vkCmdDraw(_ptr, vtxCount, instCount, firstVtx, firstInst);
    }

    void CommandBuffer::End()
    {
        VkResult result = vkEndCommandBuffer(_ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Failed ending command buffer!");
    }
} // namespace Lina::Graphics
