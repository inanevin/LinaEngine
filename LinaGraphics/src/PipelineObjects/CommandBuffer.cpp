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

#include "PipelineObjects/CommandBuffer.hpp"
#include "PipelineObjects/PipelineLayout.hpp"
#include "PipelineObjects/DescriptorSet.hpp"
#include "Core/Backend.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    void CommandBuffer::Create(VkCommandPool_T* pool)
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
    }

    void CommandBuffer::Reset(bool releaseResources)
    {
        VkResult result = vkResetCommandBuffer(_ptr, releaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Resetting command buffer failed!");
    }

    void CommandBuffer::Begin(uint32 flags)
    {
        VkCommandBufferBeginInfo beginInfo = VkCommandBufferBeginInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = nullptr,
            .flags            = flags,
            .pInheritanceInfo = nullptr,
        };

        VkResult result = vkBeginCommandBuffer(_ptr, &beginInfo);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Failed to begin!");
    }

    void CommandBuffer::BindVertexBuffers(uint32 firstBinding, uint32 bindingCount, VkBuffer_T* buffer, uint64* offset)
    {
        vkCmdBindVertexBuffers(_ptr, firstBinding, bindingCount, &buffer, offset);
    }

    void CommandBuffer::BindDescriptorSets(PipelineBindPoint bindPoint, VkPipelineLayout_T* pLayout, uint32 firstSet, uint32 setCount, DescriptorSet* sets)
    {
        Vector<VkDescriptorSet> _sets;

        for (uint32 i = 0; i < setCount; i++)
            _sets.push_back(sets[i]._ptr);

        vkCmdBindDescriptorSets(_ptr, GetPipelineBindPoint(bindPoint), pLayout, firstSet, setCount, _sets.data(), 0, nullptr);
    }

    void CommandBuffer::PushConstants(VkPipelineLayout_T* pipelineLayout, uint32 stageFlags, uint32 offset, uint32 size, void* constants)
    {
        vkCmdPushConstants(_ptr, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), constants);
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
