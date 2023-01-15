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

#include "Graphics/PipelineObjects/CommandBuffer.hpp"
#include "Graphics/PipelineObjects/PipelineLayout.hpp"
#include "Graphics/PipelineObjects/DescriptorSet.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Log/Log.hpp"
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

    void CommandBuffer::Reset(bool releaseResources) const
    {
        VkResult result = vkResetCommandBuffer(_ptr, releaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Resetting command buffer failed!");
    }

    void CommandBuffer::Begin(uint32 flags) const
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

    void CommandBuffer::CMD_BindVertexBuffers(uint32 firstBinding, uint32 bindingCount, VkBuffer_T* buffer, uint64* offset) const
    {
        vkCmdBindVertexBuffers(_ptr, firstBinding, bindingCount, &buffer, offset);
    }

    void CommandBuffer::CMD_BindIndexBuffers(VkBuffer_T* buffer, uint64 offset, IndexType indexType) const
    {
        vkCmdBindIndexBuffer(_ptr, buffer, offset, GetIndexType(indexType));
    }

    void CommandBuffer::CMD_BindDescriptorSets(PipelineBindPoint bindPoint, VkPipelineLayout_T* pLayout, uint32 firstSet, uint32 setCount, DescriptorSet* sets, uint32 dynamicOffsetCount, uint32* dynamicOffsets) const
    {
        Vector<VkDescriptorSet> _sets;
        _sets.reserve(setCount);

        for (uint32 i = 0; i < setCount; i++)
            _sets.push_back(sets[i]._ptr);

        vkCmdBindDescriptorSets(_ptr, GetPipelineBindPoint(bindPoint), pLayout, firstSet, setCount, _sets.data(), dynamicOffsetCount, dynamicOffsets);
    }

    void CommandBuffer::CMD_PushConstants(VkPipelineLayout_T* pipelineLayout, uint32 stageFlags, uint32 offset, uint32 size, void* constants) const
    {
        vkCmdPushConstants(_ptr, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), constants);
    }

    void CommandBuffer::CMD_Draw(uint32 vtxCount, uint32 instCount, uint32 firstVtx, uint32 firstInst) const
    {
        vkCmdDraw(_ptr, vtxCount, instCount, firstVtx, firstInst);
    }

    void CommandBuffer::CMD_DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) const
    {
        vkCmdDrawIndexed(_ptr, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandBuffer::CMD_DrawIndexedIndirect(VkBuffer_T* buffer, uint64 offset, uint32 drawCount, uint32 stride) const
    {
        vkCmdDrawIndexedIndirect(_ptr, buffer, offset, drawCount, stride);
    }

    void CommandBuffer::CMD_PipelineBarrier(
        uint32 srcStageFlags, uint32 dstStageFlags, uint32 dependencyFlags, const Vector<DefaultMemoryBarrier>& barriers, const Vector<BufferMemoryBarrier>& bufferBarriers, const Vector<ImageMemoryBarrier>& imageBarriers) const
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

        vkCmdPipelineBarrier(_ptr, srcStageFlags, dstStageFlags, dependencyFlags, barriersSize, _barriers.data(), bufferBarriersSize, _bufferBarriers.data(), imageBarriersSize, _imageBarriers.data());
    }

    void CommandBuffer::CMD_CopyBuffer(VkBuffer_T* src, VkBuffer_T* dst, const Vector<BufferCopy>& regions) const
    {

        Vector<VkBufferCopy> _regions;

        for (auto& bc : regions)
            _regions.push_back(VulkanUtility::GetBufferCopy(bc));

        vkCmdCopyBuffer(_ptr, src, dst, static_cast<uint32>(regions.size()), _regions.data());
    }

    void CommandBuffer::CMD_CopyBufferToImage(VkBuffer_T* src, VkImage_T* dst, ImageLayout layout, const Vector<BufferImageCopy>& copy) const
    {
        Vector<VkBufferImageCopy> _copy;
        const uint32              size = static_cast<uint32>(copy.size());
        _copy.reserve(size);

        for (auto& c : copy)
            _copy.push_back(VulkanUtility::GetBufferImageCopy(c));

        vkCmdCopyBufferToImage(_ptr, src, dst, GetImageLayout(layout), size, _copy.data());
    }

    void CommandBuffer::CMD_SetViewport(const Viewport& vp) const
    {
        VkViewport _vp = VulkanUtility::GetViewport(vp);
        vkCmdSetViewport(_ptr, 0, 1, &_vp);
    }

    void CommandBuffer::CMD_SetScissors(const Recti& rect) const
    {
        VkRect2D _scissors = VulkanUtility::GetRect(rect);
        vkCmdSetScissor(_ptr, 0, 1, &_scissors);
    }

    void CommandBuffer::CMD_BlitImage(VkImage_T* src, ImageLayout srcLayout, VkImage_T* dest, ImageLayout destLayout, Vector<ImageBlit>& regions, Filter filter) const
    {
        Vector<VkImageBlit> _regions;

        for (auto& r : regions)
        {
            VkImageBlit blit;
            blit.dstOffsets[0]  = VulkanUtility::GetOffset3D(r.dstOffsets[0]);
            blit.dstOffsets[1]  = VulkanUtility::GetOffset3D(r.dstOffsets[1]);
            blit.srcOffsets[0]  = VulkanUtility::GetOffset3D(r.srcOffsets[0]);
            blit.srcOffsets[1]  = VulkanUtility::GetOffset3D(r.srcOffsets[1]);
            blit.dstSubresource = VulkanUtility::GetImageSubresourceLayers(r.dstRange);
            blit.srcSubresource = VulkanUtility::GetImageSubresourceLayers(r.srcRange);
            _regions.push_back(blit);
        }

        vkCmdBlitImage(_ptr, src, GetImageLayout(srcLayout), dest, GetImageLayout(destLayout), static_cast<uint32>(regions.size()), &_regions[0], GetFilter(filter));
    }

    void CommandBuffer::CMD_BeginRendering(RenderingInfo& info) const
    {
        VkRenderingInfo                   _renderingInfo = VulkanUtility::GetRenderingInfo(info);
        VkRenderingAttachmentInfo         _depthInfo;
        VkRenderingAttachmentInfo         _stencilInfo;
        Vector<VkRenderingAttachmentInfo> _colorInfo;

        if (info.useDepthAttachment)
            _depthInfo = VulkanUtility::GetRenderingAttachmentInfo(info.depthAttachment);

        if (info.useStencilAttachment)
            _stencilInfo = VulkanUtility::GetRenderingAttachmentInfo(info.stencilAttachment);

        for (auto& ci : info.colorAttachments)
            _colorInfo.push_back(VulkanUtility::GetRenderingAttachmentInfo(ci));

        _renderingInfo.colorAttachmentCount = static_cast<uint32>(_colorInfo.size());
        _renderingInfo.pColorAttachments    = &_colorInfo[0];
        _renderingInfo.pDepthAttachment     = info.useDepthAttachment ? &_depthInfo : nullptr;
        _renderingInfo.pStencilAttachment   = info.useStencilAttachment ? &_stencilInfo : nullptr;
        pfn_vkCmdBeginRenderingKHR(_ptr, &_renderingInfo);
    }

    void CommandBuffer::CMD_BeginRenderingDefault(VkImageView_T* colorImageView, VkImageView_T* depthImageView, const Recti& renderArea, const Color& color) const
    {
        ClearValue clearValue = ClearValue{
            .clearColor = color,
            .isColor    = true,
        };

        ClearValue clearValueDepth = ClearValue{
            .isColor = false,
            .depth   = 1.0f,
            .stencil = 0,
        };

        RenderingAttachmentInfo colorAttachment = RenderingAttachmentInfo{
            .imageView   = colorImageView,
            .imageLayout = ImageLayout::AttachmentOptimal,
            .loadOp      = LoadOp::Clear,
            .storeOp     = StoreOp::Store,
            .clearValue  = clearValue,
        };

        RenderingAttachmentInfo depthAttachment = RenderingAttachmentInfo{
            .imageView   = depthImageView,
            .imageLayout = ImageLayout::DepthStencilOptimal,
            .loadOp      = LoadOp::Clear,
            .storeOp     = StoreOp::Store,
            .clearValue  = clearValueDepth,
        };

        RenderingInfo renderingInfo = RenderingInfo{
            .renderArea         = renderArea,
            .layerCount         = 1,
            .depthAttachment    = depthAttachment,
            .useDepthAttachment = true,
        };

        renderingInfo.colorAttachments.push_back(colorAttachment);
        CMD_BeginRendering(renderingInfo);
    }

    void CommandBuffer::CMD_BeginRenderingDefault(VkImageView_T* colorImageView, const Recti& renderArea, const Color& color) const
    {
        ClearValue clearValue = ClearValue{
            .clearColor = color,
            .isColor    = true,
        };

        RenderingAttachmentInfo colorAttachment = RenderingAttachmentInfo{
            .imageView   = colorImageView,
            .imageLayout = ImageLayout::AttachmentOptimal,
            .loadOp      = LoadOp::Clear,
            .storeOp     = StoreOp::Store,
            .clearValue  = clearValue,
        };

        RenderingInfo renderingInfo = RenderingInfo{
            .renderArea         = renderArea,
            .layerCount         = 1,
            .useDepthAttachment = false,
        };

        renderingInfo.colorAttachments.push_back(colorAttachment);
        CMD_BeginRendering(renderingInfo);
    }

    void CommandBuffer::CMD_EndRendering() const
    {
        pfn_vkCmdEndRenderingKHR(_ptr);
    }

    void CommandBuffer::CMD_ImageTransition(VkImage_T* img, ImageLayout from, ImageLayout to, ImageAspectFlags aspectFlags, AccessFlags srcAccessFlags, AccessFlags destAccessFlags, uint32 srcStage, uint32 dstStage, uint32 mipLevels, uint32 baseMip) const
    {
        ImageSubresourceRange range = ImageSubresourceRange{
            .aspectFlags    = GetImageAspectFlags(aspectFlags),
            .baseMipLevel   = 0,
            .levelCount     = mipLevels,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };

        ImageMemoryBarrier imageBarrierToTransfer = ImageMemoryBarrier{
            .srcAccessMask    = GetAccessFlags(srcAccessFlags),
            .dstAccessMask    = GetAccessFlags(destAccessFlags),
            .oldLayout        = from,
            .newLayout        = to,
            .img              = img,
            .subresourceRange = range,
        };

        Vector<ImageMemoryBarrier> imageBarriers;
        imageBarriers.push_back(imageBarrierToTransfer);

        CMD_PipelineBarrier(srcStage, dstStage, 0, {}, {}, imageBarriers);
    }

    void CommandBuffer::CMD_ImageTransition(
        VkImage_T* img, ImageLayout from, ImageLayout to, ImageAspectFlags aspectFlags, AccessFlags srcAccessFlags, AccessFlags destAccessFlags, PipelineStageFlags srcStage, PipelineStageFlags dstStage, uint32 mipLevels, uint32 baseMip) const
    {
        CMD_ImageTransition(img, from, to, aspectFlags, srcAccessFlags, destAccessFlags, GetPipelineStageFlags(srcStage), GetPipelineStageFlags(dstStage), mipLevels, baseMip);
    }

    void CommandBuffer::End() const
    {
        VkResult result = vkEndCommandBuffer(_ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Buffer] -> Failed ending command buffer!");
    }
} // namespace Lina::Graphics
