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

#include "Graphics/PipelineObjects/DescriptorSet.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/PipelineObjects/DescriptorPool.hpp"
#include "Graphics/PipelineObjects/Buffer.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Log/Log.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    void DescriptorSet::Create(const DescriptorSetLayout& layout)
    {
        // allocate one descriptor set for each frame
        VkDescriptorSetAllocateInfo allocInfo = {
            .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext              = nullptr,
            .descriptorPool     = pool,
            .descriptorSetCount = setCount,
            .pSetLayouts        = &layout._ptr,
        };

        VkResult res = vkAllocateDescriptorSets(Backend::Get()->GetDevice(), &allocInfo, &_ptr);
        LINA_ASSERT(res == VK_SUCCESS, "[Descriptor Set] -> Could not allocate descriptor set!");
    }

    void DescriptorSet::BeginUpdate()
    {
        _writes.clear();
    }

    void DescriptorSet::AddBufferUpdate(Buffer& buffer, size_t range, uint32 binding, DescriptorType type)
    {
        WriteDescriptorSet write = WriteDescriptorSet{
            .buffer          = buffer._ptr,
            .offset          = 0,
            .range           = range,
            .dstSet          = _ptr,
            .dstBinding      = binding,
            .descriptorCount = 1,
            .descriptorType  = type,
        };

        _writes.push_back(write);
    }

    void DescriptorSet::AddTextureUpdate(uint32 binding, Texture* txt)
    {

        WriteDescriptorSet textureWrite = WriteDescriptorSet{
            .dstSet          = _ptr,
            .dstBinding      = binding,
            .descriptorCount = 1,
            .descriptorType  = DescriptorType::CombinedImageSampler,
            .imageView       = txt->GetImage()._ptrImgView,
            .imageLayout     = ImageLayout::ShaderReadOnlyOptimal,
            .sampler         = txt->GetSampler()._ptr,
        };

        _writes.push_back(textureWrite);
    }

    void DescriptorSet::SendUpdate()
    {
        DescriptorSet::UpdateDescriptorSets(_writes);
        _writes.clear();
    }

    void DescriptorSet::UpdateDescriptorSets(const Vector<WriteDescriptorSet>& v)
    {
        Vector<VkWriteDescriptorSet>   _setWrites;
        Vector<VkDescriptorBufferInfo> _bufInfos;
        Vector<VkDescriptorImageInfo>  _imgInfos;

        _bufInfos.reserve(v.size());

        for (auto& write : v)
        {
            if (write.buffer != nullptr)
            {
                VkDescriptorBufferInfo binfo = VkDescriptorBufferInfo{.buffer = write.buffer, .offset = write.offset, .range = write.range};

                _bufInfos.push_back(binfo);
            }

            if (write.imageView != nullptr)
            {
                VkDescriptorImageInfo iInfo = VkDescriptorImageInfo{
                    .sampler     = write.sampler,
                    .imageView   = write.imageView,
                    .imageLayout = GetImageLayout(write.imageLayout),
                };

                _imgInfos.push_back(iInfo);
            }

            VkWriteDescriptorSet setWrite = VkWriteDescriptorSet{
                .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext           = nullptr,
                .dstSet          = write.dstSet,
                .dstBinding      = write.dstBinding,
                .descriptorCount = write.descriptorCount,
                .descriptorType  = GetDescriptorType(write.descriptorType),
            };

            if (write.buffer != nullptr)
                setWrite.pBufferInfo = &_bufInfos[_bufInfos.size() - 1];

            if (write.imageView != nullptr)
                setWrite.pImageInfo = &_imgInfos[_imgInfos.size() - 1];

            _setWrites.push_back(setWrite);
        }

        vkUpdateDescriptorSets(Backend::Get()->GetDevice(), static_cast<uint32>(_setWrites.size()), _setWrites.data(), 0, nullptr);
    }
} // namespace Lina::Graphics
