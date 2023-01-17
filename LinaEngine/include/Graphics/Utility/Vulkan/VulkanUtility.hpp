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

#ifndef VulkanUtility_HPP
#define VulkanUtility_HPP

#include "Graphics/Core/GraphicsCommon.hpp"
#include "Data/Vector.hpp"
#include "Math/Rect.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    struct Attachment;
    class RenderPass;
    class Texture;
    class Sampler;
    class Image;
    class DeletionQueue;
    
    class VulkanUtility
    {
    public:
        static VkAttachmentDescription CreateAttachmentDescription(const Attachment& att);

        // Pipeline
        static VkPipelineShaderStageCreateInfo        CreatePipelineShaderStageCreateInfo(ShaderStage stage, VkShaderModule shaderModule);
        static VkPipelineVertexInputStateCreateInfo   CreatePipelineVertexInputStateCreateInfo(const Vector<VkVertexInputBindingDescription>& bindingDescs, const Vector<VkVertexInputAttributeDescription>& attDescs);
        static VkPipelineInputAssemblyStateCreateInfo CreatePipelineInputAssemblyCreateInfo(Topology top, bool primitiveRestart = false);
        static VkPipelineRasterizationStateCreateInfo CreatePipelineRasterStateCreateInfo(PolygonMode pm, CullMode mode, FrontFace frontFace);
        static VkPipelineMultisampleStateCreateInfo   CreatePipelineMSAACreateInfo();
        static VkPipelineColorBlendAttachmentState    CreatePipelineBlendAttachmentState(ColorBlendAttachmentState blendAtt);
        static VkPipelineDepthStencilStateCreateInfo  CreatePipelineDepthStencilStateCreateInfo(bool depthTest, bool depthWrite, CompareOp op);

        // Render Pass
        static void     GetDefaultPassSampler(Sampler& sampler);
        static Texture* CreateDefaultPassTextureColor(bool useExplicitSize = false, int width = 0, int height = 0);
        static Texture* CreateDefaultPassTextureDepth(bool useExplicitSize = false, int width = 0, int height = 0);
        static void     CreateDefaultPassImageDepth(Image& img, bool useExplicitSize = false, int width = 0, int height = 0);
        static void     CreateDefaultPassImageColor(Image& img, bool useExplicitSize = false, int width = 0, int height = 0);
        static void     CreateMainRenderPass(RenderPass& pass);
        static void     CreatePresentRenderPass(RenderPass& pass);

        // InputDesc
        static VertexInputDescription GetVertexDescription();
        static VertexInputDescription GetEmptyVertexDescription();
        static VertexInputDescription GetGUIVertexDescription();
        static void                   GetDescriptionStructs(const VertexInputDescription& desc, Vector<VkVertexInputBindingDescription>& bindingDescs, Vector<VkVertexInputAttributeDescription>& attDescs);

        // Image
        static VkImageCreateInfo     GetImageCreateInfo(Format format, uint32 usageFlags, ImageTiling tiling, Extent3D extent, ImageLayout initialLayout, SharingMode sharingMode, uint32 mipLevels);
        static VkImageViewCreateInfo GetImageViewCreateInfo(VkImage img, Format format, ImageSubresourceRange subres);

        // Pass
        static VkSubpassDependency GetSubpassDependency(SubPassDependency& dependency);

        // Descriptor
        static VkDescriptorSetLayoutBinding GetDescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& binding);

        // Buffers
        static VkBufferCopy             GetBufferCopy(const BufferCopy& copy);
        static VkBufferImageCopy        GetBufferImageCopy(const BufferImageCopy& copy);
        static VkImageSubresourceLayers GetImageSubresourceLayers(const ImageSubresourceRange& r);

        // Barriers
        static VkMemoryBarrier       GetMemoryBarrier(const DefaultMemoryBarrier& bar);
        static VkBufferMemoryBarrier GetBufferMemoryBarrier(const BufferMemoryBarrier& bar);
        static VkImageMemoryBarrier  GetImageMemoryBarrier(const ImageMemoryBarrier& bar);

        // Viewport
        static VkViewport GetViewport(const Viewport& vp);
        static VkRect2D   GetRect(const Recti& rect);

        // Others
        static VkExtent3D   GetExtent3D(const Extent3D& e);
        static VkOffset3D   GetOffset3D(const Offset3D& e);
        static Offset3D     GetOffset3D(const Extent3D& e);
        static size_t       PadUniformBufferSize(size_t originalSize);
        static VkClearValue GetClearValue(const ClearValue& c);

        // Rendering
        static VkRenderingAttachmentInfo GetRenderingAttachmentInfo(const RenderingAttachmentInfo& inf);
        static VkRenderingInfo           GetRenderingInfo(const RenderingInfo& inf);
    };

    // Functions
    extern PFN_vkCmdBeginRenderingKHR g_vkCmdBeginRenderingKHR;
    extern PFN_vkCmdEndRenderingKHR   g_vkCmdEndRenderingKHR;

#define pfn_vkCmdBeginRenderingKHR g_vkCmdBeginRenderingKHR
#define pfn_vkCmdEndRenderingKHR   g_vkCmdEndRenderingKHR

} // namespace Lina::Graphics

#endif
