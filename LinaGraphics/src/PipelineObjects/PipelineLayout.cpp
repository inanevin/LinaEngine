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

#include "PipelineObjects/PipelineLayout.hpp"
#include "Core/Backend.hpp"
#include "Core/RenderEngine.hpp"
#include "Log/Log.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{

    void PipelineLayout::Create()
    {
        Vector<VkPushConstantRange> ranges;

        for (auto& pc : pushConstantRanges)
        {
            VkPushConstantRange p = VkPushConstantRange{
                .stageFlags = static_cast<VkShaderStageFlags>(pc.stageFlags),
                .offset     = pc.offset,
                .size       = pc.size,
            };
            ranges.push_back(p);
        }

        Vector<VkDescriptorSetLayout> setLayouts;

        for (auto& sl : _setLayoutPtrs)
            setLayouts.push_back(sl);

        VkPipelineLayoutCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,

            // empty defaults
            .flags                  = 0,
            .setLayoutCount         = static_cast<uint32>(setLayouts.size()),
            .pSetLayouts            = setLayouts.data(),
            .pushConstantRangeCount = static_cast<uint32>(ranges.size()),
            .pPushConstantRanges    = ranges.data(),
        };

        VkResult res = vkCreatePipelineLayout(Backend::Get()->GetDevice(), &info, Backend::Get()->GetAllocator(), &_ptr);
        LINA_ASSERT(res == VK_SUCCESS, "[Pipeline Layout] -> Creating pipeline layout failed!");

        VkPipelineLayout_T* ptr = _ptr;
        RenderEngine::Get()->GetMainDeletionQueue().Push([ptr]() {
            vkDestroyPipelineLayout(Backend::Get()->GetDevice(), ptr, Backend::Get()->GetAllocator());
        });

       // pushConstantRanges.clear();
       // _setLayoutPtrs.clear();
    }

    PipelineLayout& PipelineLayout::AddPushConstant(const PushConstantRange& r)
    {
        pushConstantRanges.push_back(r);
        return *this;
    }

    PipelineLayout& PipelineLayout::AddDescriptorSetLayout(const DescriptorSetLayout& l)
    {
        _setLayoutPtrs.push_back(l._ptr);
        return *this;
    }

} // namespace Lina::Graphics
