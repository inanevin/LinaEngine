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

#include "PipelineObjects/DescriptorSetLayout.hpp"
#include "Core/Backend.hpp"
#include "Core/RenderEngine.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    void DescriptorSetLayout::Create()
    {
        Vector<VkDescriptorSetLayoutBinding> _bindings;

        for (auto& b : bindings)
        {
            VkDescriptorSetLayoutBinding bindingDesc = VulkanUtility::GetDescriptorSetLayoutBinding(b);
            _bindings.push_back(bindingDesc);
        }

        VkDescriptorSetLayoutCreateInfo setinfo = VkDescriptorSetLayoutCreateInfo{
            .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .flags        = GetDescriptorSetCreateFlags(flags),
            .bindingCount = static_cast<uint32>(_bindings.size()),
            .pBindings    = _bindings.data(),
        };

        Vector<VkDescriptorBindingFlags> _bindingFlags;

        for (auto& b : bindingFlags)
            _bindingFlags.push_back(GetDescriptorLayoutBindingFlags(b));

        VkDescriptorSetLayoutBindingFlagsCreateInfo bf = VkDescriptorSetLayoutBindingFlagsCreateInfo{
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .pNext         = nullptr,
            .bindingCount  = static_cast<uint32>(_bindingFlags.size()),
            .pBindingFlags = _bindingFlags.data(),
        };

        if (!bindingFlags.empty())
            setinfo.pNext = &bf;
        else
            setinfo.pNext = nullptr;

        VkResult res = vkCreateDescriptorSetLayout(Backend::Get()->GetDevice(), &setinfo, Backend::Get()->GetAllocator(), &_ptr);
        LINA_ASSERT(res == VK_SUCCESS, "[Descriptor Set Layout] -> Could not create layout!");

        VkDescriptorSetLayout_T* ptr = _ptr;
        RenderEngine::Get()->GetMainDeletionQueue().Push([ptr]() { vkDestroyDescriptorSetLayout(Backend::Get()->GetDevice(), ptr, Backend::Get()->GetAllocator()); });
    }

    DescriptorSetLayout& DescriptorSetLayout::AddBinding(DescriptorSetLayoutBinding binding)
    {
        bindings.push_back(binding);
        return *this;
    }
    DescriptorSetLayout& DescriptorSetLayout::AddBindingFlag(DescriptorSetLayoutBindingFlags flag)
    {
        bindingFlags.push_back(flag);
        return *this;
    }
} // namespace Lina::Graphics
