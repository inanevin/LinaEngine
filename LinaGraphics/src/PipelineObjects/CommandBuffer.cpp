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
#include "Utility/Vulkan/VulkanUtility.hpp"
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

    void CommandBuffer::CMD_BindVertexBuffers(uint32 firstBinding, uint32 bindingCount, VkBuffer_T* buffer, uint64* offset)
    {
        vkCmdBindVertexBuffers(_ptr, firstBinding, bindingCount, &buffer, offset);
    }

    void CommandBuffer::CMD_BindIndexBuffers(VkBuffer_T* buffer, uint64 offset, IndexType indexType)
    {
        vkCmdBindIndexBuffer(_ptr, buffer, offset, GetIndexType(indexType));
    }

    void CommandBuffer::CMD_BindDescriptorSets(PipelineBindPoint bindPoint, VkPipelineLayout_T* pLayout, uint32 firstSet, uint32 setCount, DescriptorSet* sets, uint32 dynamicOffsetCount, uint32* dynamicOffsets)
    {
        Vector<VkDescriptorSet> _sets;
        _sets.reserve(setCount);

        for (uint32 i = 0; i < setCount; i++)
            _sets.push_back(sets[i]._ptr);

        vkCmdBindDescriptorSets(_ptr, GetPipelineBindPoint(bindPoint), pLayout, firstSet, setCount, _sets.data(), dynamicOffsetCount, dynamicOffsets);
    }

    void CommandBuffer::CMD_PushConstants(VkPipelineLayout_T* pipelineLayout, uint32 stageFlags, uint32 offset, uint32 size, void* constants)
    {
        vkCmdPushConstants(_ptr, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), constants);
    }

    void CommandBuffer::CMD_Draw(uint32 vtxCount, uint32 instCount, uint32 firstVtx, uint32 firstInst)
    {
        vkCmdDraw(_ptr, vtxCount, instCount, firstVtx, firstInst);
    }

    void CommandBuffer::CMD_DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance)
    {
        vkCmdDrawIndexed(_ptr, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandBuffer::CMD_DrawIndexedIndirect(VkBuffer_T* buffer, uint64 offset, uint32 drawCount, uint32 stride)
    {
        vkCmdDrawIndexedIndirect(_ptr, buffer, offset, drawCount, stride);
    }

    void CommandBuffer::CMD_PipelineBarrier(PipelineStageFlags srcStageFlags, PipelineStageFlags dstStageFlags, uint32 dependencyFlags, const Vector<DefaultMemoryBarrier>& barriers, const Vector<BufferMemoryBarrier>& bufferBarriers, const Vector<ImageMemoryBarrier>& imageBarriers)
    {
        Vector<VkMemoryBarrier>       _barriers;
        Vector<VkBufferMemoryBarrier> _bufferBarriers;
        Vector<VkImageMemoryBarrier>  _imageBarriers;
        const uint32                  barriersSize       = static_cast<uint32>(barriers.size());
        const uint32                  bufferBarriersSize = static_cast<uint32>(bufferBarriers.size());
        const uint32                  imageBarriersSize  = static_cast<uint32>(imageBarriers.size());
        _barriers.reserve(barriersSize);
        _bufferBarriers.reserve(bufferBarriersSize);
        _imageBarriers.reserve(imageBarriersSize);

        for (auto& b : barriers)
            _barriers.push_back(VulkanUtility::GetMemoryBarrier(b));

        for (auto& b : bufferBarriers)
            _bufferBarriers.push_back(VulkanUtility::GetBufferMemoryBarrier(b));

        for (auto& b : imageBarriers)
            _imageBarriers.push_back(VulkanUtility::GetImageMemoryBarrier(b));

        vkCmdPipelineBarrier(_ptr, GetPipelineStageFlags(srcStageFlags), GetPipelineStageFlags(dstStageFlags), dependencyFlags, barriersSize, _barriers.data(), bufferBarriersSize, _bufferBarriers.data(), imageBarriersSize, _imageBarriers.data());
    }

    void CommandBuffer::CMD_CopyBuffer(VkBuffer_T* src, VkBuffer_T* dst, const Vector<BufferCopy>& regions)
    {

        Vector<VkBufferCopy> _regions;

        for (auto& bc : regions)
            _regions.push_back(VulkanUtility::GetBufferCopy(bc));

        vkCmdCopyBuffer(_ptr, src, dst, static_cast<uint32>(regions.size()), _regions.data());
    }

    void CommandBuffer::CMD_CopyBufferToImage(VkBuffer_T* src, VkImage_T* dst, ImageLayout layout, const Vector<BufferImageCopy>& copy)
    {
        Vector<VkBufferImageCopy> _copy;
        const uint32              size = static_cast<uint32>(copy.size());
        _copy.reserve(size);

        for (auto& c : copy)
            _copy.push_back(VulkanUtility::GetBufferImageCopy(c));

        vkCmdCopyBufferToImage(_ptr, src, dst, GetImageLayout(layout), size, _copy.data());
    }

    void CommandBuffer::CMD_SetViewport(Viewport& vp)
    {
        VkViewport _vp = VulkanUtility::GetViewport(vp);
        vkCmdSetViewport(_ptr, 0, 1, &_vp);
    }

    void CommandBuffer::CMD_SetScissors(Recti& rect)
    {
        VkRect2D   _scissors = VulkanUtility::GetRect(rect);
        vkCmdSetScissor(_ptr, 0, 1, &_scissors);
    }

    void CommandBuffer::End()
    {
        VkResult result = vkEndCommandBuffer(_ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Failed ending command buffer!");
    }
} // namespace Lina::Graphics
