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

#ifndef Pipeline_HPP
#define Pipeline_HPP

#include "Core/GraphicsCommon.hpp"
#include "Math/Rect.hpp"

struct VkPipeline_T;
struct VkRenderPass_T;
struct VkPipelineLayout_T;

namespace Lina::Graphics
{
    class RenderPass;
    class Shader;
    class PipelineLayout;
    class CommandBuffer;

    class Pipeline
    {
    public:
        void      Create();
        Pipeline& SetShader(Shader* shader);
        Pipeline& SetRenderPass(const RenderPass& rp);
        Pipeline& SetLayout(const PipelineLayout& layout);

        void Bind(const CommandBuffer& cmd, PipelineBindPoint bindpoint);

        // Desc
        PipelineType              pipelineType      = PipelineType::Default;
        bool                      depthTestEnabled  = true;
        bool                      depthWriteEnabled = true;
        CompareOp                 depthCompareOp    = CompareOp::LEqual;
        Topology                  topology          = Topology::TriangleFan;
        PolygonMode               polygonMode       = PolygonMode::Fill;
        CullMode                  cullMode          = CullMode::None;
        FrontFace                 frontFace         = FrontFace::ClockWise;
        ColorBlendAttachmentState blendAttachment;
        bool                      colorBlendLogicOpEnabled = false;
        LogicOp                   colorBlendLogicOp        = LogicOp::Copy;

        // Runtime
        Shader*             _shader     = nullptr;
        VkRenderPass_T*     _renderPass = nullptr;
        VkPipelineLayout_T* _layout     = nullptr;
        VkPipeline_T*       _ptr        = nullptr;
    };
} // namespace Lina::Graphics

#endif
