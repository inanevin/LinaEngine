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

#include "Utility/Vulkan/VulkanUtility.hpp"
#include "Data/Vertex.hpp"
#include "Core/Backend.hpp"

namespace Lina::Graphics
{
    VkAttachmentDescription VulkanUtility::CreateAttachmentDescription(const Attachment& att)
    {
        return VkAttachmentDescription{
            .flags          = att.flags,
            .format         = GetFormat(att.format),
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = GetLoadOp(att.loadOp),
            .storeOp        = GetStoreOp(att.storeOp),
            .stencilLoadOp  = GetLoadOp(att.stencilLoadOp),
            .stencilStoreOp = GetStoreOp(att.stencilStoreOp),
            .initialLayout  = GetImageLayout(att.initialLayout),
            .finalLayout    = GetImageLayout(att.finalLayout),
        };
    }
    VkPipelineShaderStageCreateInfo VulkanUtility::CreatePipelineShaderStageCreateInfo(ShaderStage stage, VkShaderModule shaderModule)
    {
        VkPipelineShaderStageCreateInfo info = VkPipelineShaderStageCreateInfo{
            .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext  = nullptr,
            .stage  = static_cast<VkShaderStageFlagBits>(GetShaderStage(stage)),
            .module = shaderModule,
            .pName  = "main",
        };

        return info;
    }
    VkPipelineVertexInputStateCreateInfo VulkanUtility::CreatePipelineVertexInputStateCreateInfo(const Vector<VkVertexInputBindingDescription>& bindingDescs, const Vector<VkVertexInputAttributeDescription>& attDescs)
    {
        VkPipelineVertexInputStateCreateInfo info = VkPipelineVertexInputStateCreateInfo{
            .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext                           = nullptr,
            .vertexBindingDescriptionCount   = static_cast<uint32>(bindingDescs.size()),
            .pVertexBindingDescriptions      = bindingDescs.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32>(attDescs.size()),
            .pVertexAttributeDescriptions    = attDescs.data(),
        };

        return info;
    }
    VkPipelineInputAssemblyStateCreateInfo VulkanUtility::CreatePipelineInputAssemblyCreateInfo(Topology top, bool primitiveRestart)
    {
        VkPipelineInputAssemblyStateCreateInfo info = VkPipelineInputAssemblyStateCreateInfo{
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext                  = nullptr,
            .topology               = GetTopology(top),
            .primitiveRestartEnable = primitiveRestart ? VK_TRUE : VK_FALSE,
        };

        return info;
    }

    VkPipelineRasterizationStateCreateInfo VulkanUtility::CreatePipelineRasterStateCreateInfo(PolygonMode pm, CullMode cm)
    {
        VkPipelineRasterizationStateCreateInfo info = VkPipelineRasterizationStateCreateInfo{
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext                   = nullptr,
            .depthClampEnable        = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode             = GetPolygonMode(pm),
            .cullMode                = GetCullMode(cm),
            .frontFace               = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable         = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp          = 0.0f,
            .depthBiasSlopeFactor    = 0.0f,
            .lineWidth               = 1.0f,
        };
        return info;
    }
    VkPipelineMultisampleStateCreateInfo VulkanUtility::CreatePipelineMSAACreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo info = VkPipelineMultisampleStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            // multisampling defaulted to no multisampling (1 sample per pixel)
            .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable   = VK_FALSE,
            .minSampleShading      = 1.0f,
            .pSampleMask           = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable      = VK_FALSE,

        };
        return info;
    }

    VkPipelineColorBlendAttachmentState VulkanUtility::CreatePipelineBlendAttachmentState()
    {
        VkPipelineColorBlendAttachmentState info = VkPipelineColorBlendAttachmentState{
            .blendEnable    = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        return info;
    }

    VkPipelineDepthStencilStateCreateInfo VulkanUtility::CreatePipelineDepthStencilStateCreateInfo(bool depthTest, bool depthWrite, CompareOp op)
    {
        VkPipelineDepthStencilStateCreateInfo info = {
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext                 = nullptr,
            .depthTestEnable       = depthTest ? VK_TRUE : VK_FALSE,
            .depthWriteEnable      = depthWrite ? VK_TRUE : VK_FALSE,
            .depthCompareOp        = depthTest ? GetCompareOp(op) : VK_COMPARE_OP_ALWAYS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable     = VK_FALSE,
            .minDepthBounds        = 0.0f, // Optional
            .maxDepthBounds        = 1.0f, // Optional
        };

        return info;
    }

    VertexInputDescription VulkanUtility::GetVertexDescription()
    {
        VertexInputDescription description;

        VertexInputBinding mainBinding = VertexInputBinding{
            .binding   = 0,
            .stride    = sizeof(Vertex),
            .inputRate = VertexInputRate::Vertex,
        };

        description.bindings.push_back(mainBinding);

        VertexInputAttribute positionAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 0,
            .format   = Format::R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, pos),
        };

        VertexInputAttribute normalAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 1,
            .format   = Format::R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, normal),
        };

        VertexInputAttribute colorAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 2,
            .format   = Format::R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, color),
        };

        VertexInputAttribute uvAtt = VertexInputAttribute{
            .binding  = 0,
            .location = 3,
            .format   = Format::R32G32_SFLOAT,
            .offset   = offsetof(Vertex, uv),
        };

        description.attributes.push_back(positionAtt);
        description.attributes.push_back(normalAtt);
        description.attributes.push_back(colorAtt);
        description.attributes.push_back(uvAtt);

        return description;
    }
    void VulkanUtility::GetDescriptionStructs(const VertexInputDescription& desc, Vector<VkVertexInputBindingDescription>& bindingDescs, Vector<VkVertexInputAttributeDescription>& attDescs)
    {
        for (const auto& d : desc.bindings)
        {
            VkVertexInputBindingDescription binding = VkVertexInputBindingDescription{
                .binding   = d.binding,
                .stride    = d.stride,
                .inputRate = GetVertexInputRate(d.inputRate),
            };

            bindingDescs.push_back(binding);
        }

        for (const auto& a : desc.attributes)
        {
            VkVertexInputAttributeDescription att = VkVertexInputAttributeDescription{
                .location = a.location,
                .binding  = a.binding,
                .format   = GetFormat(a.format),
                .offset   = a.offset,
            };

            attDescs.push_back(att);
        }
    }

    VkImageCreateInfo VulkanUtility::GetImageCreateInfo(Format format, uint32 usageFlags, ImageTiling tiling, Extent3D extent)
    {
        VkImageCreateInfo info = VkImageCreateInfo{
            .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext       = nullptr,
            .imageType   = VK_IMAGE_TYPE_2D,
            .format      = GetFormat(format),
            .extent      = VkExtent3D{extent.width, extent.height, extent.depth},
            .mipLevels   = 1,
            .arrayLayers = 1,
            .samples     = VK_SAMPLE_COUNT_1_BIT,
            .tiling      = GetImageTiling(tiling),
            .usage       = usageFlags,
        };
        return info;
    }

    VkImageViewCreateInfo VulkanUtility::GetImageViewCreateInfo(VkImage img, Format format, uint32 aspectFlags)
    {
        VkImageSubresourceRange subResRange = VkImageSubresourceRange{
            .aspectMask     = aspectFlags,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };

        VkImageViewCreateInfo info = VkImageViewCreateInfo{
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext            = nullptr,
            .image            = img,
            .viewType         = VK_IMAGE_VIEW_TYPE_2D,
            .format           = GetFormat(format),
            .subresourceRange = subResRange,
        };

        return info;
    }


    VkSubpassDependency VulkanUtility::GetSubpassDependency(SubPassDependency& dependency)
    {
        VkSubpassDependency dep = VkSubpassDependency{
            .srcSubpass    = VK_SUBPASS_EXTERNAL,
            .dstSubpass    = dependency.dstSubpass,
            .srcStageMask  = dependency.srcStageMask,
            .dstStageMask  = dependency.dstStageMask,
            .srcAccessMask = dependency.srcAccessMask,
            .dstAccessMask = dependency.dstAccessMask,
        };

        return dep;
    }

    VkBufferCopy VulkanUtility::GetBufferCopy(const BufferCopy& copy)
    {
        VkBufferCopy c = VkBufferCopy{
            .srcOffset = copy.sourceOffset,
            .dstOffset = copy.destinationOffset,
            .size      = copy.size,
        };

        return c;
    }

    VkBufferImageCopy VulkanUtility::GetBufferImageCopy(const BufferImageCopy& copy)
    {
        VkBufferImageCopy c = VkBufferImageCopy{
            .bufferOffset      = copy.bufferOffset,
            .bufferRowLength   = copy.bufferRowLength,
            .bufferImageHeight = copy.bufferImageHeight,
            .imageSubresource  = GetImageSubresourceLayers(copy.imageSubresource),
            .imageOffset       = GetOffset3D(copy.imageOffset),
            .imageExtent       = GetExtent3D(copy.imageExtent),
        };
        return c;
    }

    VkImageSubresourceLayers VulkanUtility::GetImageSubresourceLayers(const ImageSubresourceLayers& r)
    {
        VkImageSubresourceLayers srl = VkImageSubresourceLayers{
            .aspectMask     = GetImageAspectFlags(r.aspectMask),
            .mipLevel       = r.mipLevel,
            .baseArrayLayer = r.baseArrayLayer,
            .layerCount     = r.layerCount,
        };
        return srl;
    }

    VkDescriptorSetLayoutBinding VulkanUtility::GetDescriptorSetLayoutBinding(const DescriptorSetLayoutBinding& binding)
    {
        VkDescriptorSetLayoutBinding _binding = VkDescriptorSetLayoutBinding{
            .binding         = binding.binding,
            .descriptorType  = GetDescriptorType(binding.type),
            .descriptorCount = binding.descriptorCount,
            .stageFlags      = binding.stageFlags,
        };

        return _binding;
    }

    VkMemoryBarrier VulkanUtility::GetMemoryBarrier(const DefaultMemoryBarrier& bar)
    {
        VkMemoryBarrier b = VkMemoryBarrier{
            .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .pNext         = nullptr,
            .srcAccessMask = bar.srcAccessMask,
            .dstAccessMask = bar.dstAccessMask,
        };
        return b;
    }

    VkBufferMemoryBarrier VulkanUtility::GetBufferMemoryBarrier(const BufferMemoryBarrier& bar)
    {
        VkBufferMemoryBarrier b = VkBufferMemoryBarrier{
            .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext               = nullptr,
            .srcAccessMask       = bar.srcAccessMask,
            .dstAccessMask       = bar.dstAccessMask,
            .srcQueueFamilyIndex = bar.srcQueueFamilyIndex,
            .dstQueueFamilyIndex = bar.dstQueueFamilyIndex,
            .buffer              = bar.buffer,
            .offset              = bar.offset,
            .size                = bar.size};

        return b;
    }

    VkImageMemoryBarrier VulkanUtility::GetImageMemoryBarrier(const ImageMemoryBarrier& bar)
    {
        VkImageSubresourceRange subresRange = VkImageSubresourceRange{
            .aspectMask     = GetImageAspectFlags(bar.subresourceRange.aspectMask),
            .baseMipLevel   = bar.subresourceRange.baseMipLevel,
            .levelCount     = bar.subresourceRange.levelCount,
            .baseArrayLayer = bar.subresourceRange.baseArrayLayer,
            .layerCount     = bar.subresourceRange.layerCount,
        };

        VkImageMemoryBarrier b = VkImageMemoryBarrier{

            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext               = nullptr,
            .srcAccessMask       = bar.srcAccessMask,
            .dstAccessMask       = bar.dstAccessMask,
            .oldLayout           = GetImageLayout(bar.oldLayout),
            .newLayout           = GetImageLayout(bar.newLayout),
            .srcQueueFamilyIndex = bar.srcQueueFamilyIndex,
            .dstQueueFamilyIndex = bar.dstQueueFamilyIndex,
            .image               = bar.img,
            .subresourceRange    = subresRange,
        };
        return b;
    }

    VkExtent3D VulkanUtility::GetExtent3D(Extent3D e)
    {
        VkExtent3D ex = VkExtent3D{
            .width  = e.width,
            .height = e.height,
            .depth  = e.depth,
        };

        return ex;
    }

    VkOffset3D VulkanUtility::GetOffset3D(Offset3D o)
    {
        VkOffset3D off = VkOffset3D{
            .x = o.x,
            .y = o.y,
            .z = o.z,
        };
        return off;
    }

    size_t VulkanUtility::PadUniformBufferSize(size_t originalSize)
    {
        size_t minUboAlignment = static_cast<size_t>(Backend::Get()->GetMinUniformBufferOffsetAlignment());
        size_t alignedSize     = originalSize;
        if (minUboAlignment > 0)
        {
            alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
        }
        return alignedSize;
    }

} // namespace Lina::Graphics
