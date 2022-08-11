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
#include "Core/GraphicsCommon.hpp"
#include "PipelineObjects/Attachment.hpp"
#include "Data/Vertex.hpp"

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

        description.attributes.push_back(positionAtt);
        description.attributes.push_back(normalAtt);
        description.attributes.push_back(colorAtt);

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

} // namespace Lina::Graphics
