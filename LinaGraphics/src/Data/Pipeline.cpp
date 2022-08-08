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

#include "Data/Pipeline.hpp"
#include "Core/Backend.hpp"
#include "Utility/VulkanUtility.hpp"
#include "Data/RenderPass.hpp"
#include "Data/CommandBuffer.hpp"
#include "Data/PipelineLayout.hpp"
#include "Resource/Shader.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    Pipeline Pipeline::Create()
    {
        VkViewport _viewport = VkViewport{
            .x        = viewport.x,
            .y        = viewport.y,
            .width    = viewport.width,
            .height   = viewport.height,
            .minDepth = viewport.minDepth,
            .maxDepth = viewport.maxDepth,
        };

        VkRect2D _scissor = VkRect2D{
            .offset = VkOffset2D(scissor.pos.x, scissor.pos.y),
            .extent = VkExtent2D(scissor.size.x, scissor.size.y),
        };

        VkPipelineViewportStateCreateInfo viewportState = {
            .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext         = nullptr,
            .viewportCount = 1,
            .pViewports    = &_viewport,
            .scissorCount  = 1,
            .pScissors     = &_scissor,
        };

        VkPipelineColorBlendAttachmentState _colorBlendAttachment = VulkanUtility::CreatePipelineBlendAttachmentState();

        // Write to color attachment, no blending, dummy
        VkPipelineColorBlendStateCreateInfo _colorBlending = {
            .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext           = nullptr,
            .logicOpEnable   = VK_FALSE,
            .logicOp         = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments    = &_colorBlendAttachment,
        };

        VkPipelineInputAssemblyStateCreateInfo _inputAssembly = VulkanUtility::CreatePipelineInputAssemblyCreateInfo(topology);
        VkPipelineMultisampleStateCreateInfo   _msaa          = VulkanUtility::CreatePipelineMSAACreateInfo();
        VkPipelineRasterizationStateCreateInfo _raster        = VulkanUtility::CreatePipelineRasterStateCreateInfo(polygonMode, cullMode);
        VkPipelineVertexInputStateCreateInfo   _vertexInput   = VulkanUtility::CreatePipelineVertexInputStateCreateInfo();

        Vector<VkPipelineShaderStageCreateInfo> _shaderStages;

        for (auto* shader : _shaders)
        {
            VkPipelineShaderStageCreateInfo vtxInfo  = VulkanUtility::CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, shader->GetModule(ShaderStage::Vertex));
            VkPipelineShaderStageCreateInfo fragInfo = VulkanUtility::CreatePipelineShaderStageCreateInfo(ShaderStage::Fragment, shader->GetModule(ShaderStage::Fragment));
            _shaderStages.push_back(vtxInfo);
            _shaderStages.push_back(fragInfo);

            if (shader->HasStage(ShaderStage::Geometry))
            {
                VkPipelineShaderStageCreateInfo geoInfo = VulkanUtility::CreatePipelineShaderStageCreateInfo(ShaderStage::Geometry, shader->GetModule(ShaderStage::Geometry));
                _shaderStages.push_back(geoInfo);
            }
        }

        VkGraphicsPipelineCreateInfo pipelineInfo = VkGraphicsPipelineCreateInfo{
            .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext               = nullptr,
            .stageCount          = static_cast<uint32>(_shaderStages.size()),
            .pStages             = _shaderStages.data(),
            .pVertexInputState   = &_vertexInput,
            .pInputAssemblyState = &_inputAssembly,
            .pViewportState      = &viewportState,
            .pRasterizationState = &_raster,
            .pMultisampleState   = &_msaa,
            .pColorBlendState    = &_colorBlending,
            .layout              = _layout,
            .renderPass          = _renderPass,
            .subpass             = 0,
            .basePipelineHandle  = VK_NULL_HANDLE,
        };

        VkResult res = vkCreateGraphicsPipelines(Backend::Get()->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, Backend::Get()->GetAllocator(), &_ptr);
        LINA_ASSERT(res == VK_SUCCESS, "[Command Buffer] -> Could not allocate command buffers!");

        return *this;
    }

    Pipeline Pipeline::AddShader(Shader* shader)
    {
        _shaders.push_back(shader);
        return *this;
    }

    Pipeline Pipeline::SetRenderPass(const RenderPass& rp)
    {
        _renderPass = rp._ptr;
        return *this;
    }
    Pipeline Pipeline::SetLayout(const PipelineLayout& layout)
    {
        _layout = layout._ptr;
        return *this;
    }

    void Pipeline::Bind(const CommandBuffer& cmd, PipelineBindPoint bindpoint)
    {
        vkCmdBindPipeline(cmd._ptr, GetPipelineBindPoint(bindpoint), _ptr);
    }

    void Pipeline::Destroy()
    {
        vkDestroyPipeline(Backend::Get()->GetDevice(), _ptr, Backend::Get()->GetAllocator());
    }
} // namespace Lina::Graphics
