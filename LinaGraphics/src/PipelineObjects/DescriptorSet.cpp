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

#include "PipelineObjects/DescriptorSet.hpp"
#include "Core/Backend.hpp"
#include "Core/RenderEngine.hpp"
#include "PipelineObjects/DescriptorPool.hpp"
#include "PipelineObjects/Buffer.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    void DescriptorSet::Create()
    {

        // allocate one descriptor set for each frame
        VkDescriptorSetAllocateInfo allocInfo = {
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext              = nullptr,
            .descriptorPool     = pool,
            .descriptorSetCount = setCount,
            .pSetLayouts        = _layouts.data(),
        };

        VkResult res = vkAllocateDescriptorSets(Backend::Get()->GetDevice(), &allocInfo, &_ptr);
        LINA_ASSERT(res == VK_SUCCESS, "[Descriptor Set] -> Could not allocate descriptor set!");
    }

    DescriptorSet& DescriptorSet::AddSetLayout(VkDescriptorSetLayout_T* layout)
    {
        _layouts.push_back(layout);
        return *this;
    }

    void DescriptorSet::UpdateSingle(uint32 binding, DescriptorType type, Buffer* buf, uint32 offset, size_t range)
    {
        VkDescriptorBufferInfo binfo = VkDescriptorBufferInfo{
            .buffer = buf->_ptr,
            .offset = offset,
            .range  = range};

        VkWriteDescriptorSet setWrite = VkWriteDescriptorSet{
            .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext           = nullptr,
            .dstSet          = _ptr,
            .dstBinding      = binding,
            .descriptorCount = 1,
            .descriptorType  = GetDescriptorType(type),
            .pBufferInfo     = &binfo,
        };

        vkUpdateDescriptorSets(Backend::Get()->GetDevice(), 1, &setWrite, 0, nullptr);
    }
} // namespace Lina::Graphics
