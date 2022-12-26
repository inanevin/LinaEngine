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

#include "Graphics/PipelineObjects/Buffer.hpp"
#include "Graphics/PipelineObjects/DescriptorSet.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Log/Log.hpp"
#include "Graphics/Utility/Vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{

    void Buffer::Create()
    {
        VkBufferCreateInfo bufferInfo = VkBufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .size  = size,
            .usage = bufferUsage,
        };

        VmaAllocationCreateInfo vmaallocInfo = {
            .usage         = GetMemoryUsageFlags(memoryUsage),
            .requiredFlags = GetMemoryPropertyFlags(requiredFlags),
        };

        VkResult result = vmaCreateBuffer(Backend::Get()->GetVMA(), &bufferInfo, &vmaallocInfo, &_ptr, &_allocation, nullptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Buffer] -> Could not create Buffer!");
    }

    void Buffer::Destroy()
    {
        if (_ptr == nullptr)
            return;

        vmaDestroyBuffer(Backend::Get()->GetVMA(), _ptr, _allocation);

        size        = 0;
        _ptr        = nullptr;
        _allocation = nullptr;
    }

    void Buffer::CopyInto(const void* src, size_t sz)
    {
        if (sz > size)
            Recreate(size + sz);

        void* data;
        vmaMapMemory(Backend::Get()->GetVMA(), _allocation, &data);
        MEMCPY(data, src, sz);
        vmaUnmapMemory(Backend::Get()->GetVMA(), _allocation);
        _ready = true;
    }

    void Buffer::CopyInto(unsigned char* src, size_t sz)
    {
        if (sz > size)
            Recreate(size + sz);

        void* data;
        vmaMapMemory(Backend::Get()->GetVMA(), _allocation, &data);
        MEMCPY(data, src, sz);
        vmaUnmapMemory(Backend::Get()->GetVMA(), _allocation);
        _ready = true;
    }

    void Buffer::CopyIntoPadded(const void* src, size_t sz, size_t padding)
    {
        if (sz > size)
            Recreate(size + sz);

        char* data;
        vmaMapMemory(Backend::Get()->GetVMA(), _allocation, (void**)&data);
        data += padding;
        MEMCPY(data, src, sz);
        vmaUnmapMemory(Backend::Get()->GetVMA(), _allocation);
        _ready = true;
    }

    void* Buffer::MapBuffer()
    {
        void* data;
        vmaMapMemory(Backend::Get()->GetVMA(), _allocation, &data);
        return data;
    }

    void Buffer::UnmapBuffer()
    {
        vmaUnmapMemory(Backend::Get()->GetVMA(), _allocation);
    }

    void Buffer::Recreate(size_t sz)
    {
        Destroy();
        size = sz;
        Create();

        if (boundSet != nullptr)
        {
            boundSet->BeginUpdate();
            boundSet->AddBufferUpdate(*this, size, boundBinding, boundType);
            boundSet->SendUpdate();
        }
    }

    void Buffer::ResetIfNecessary(size_t newSize)
    {
        // if(newSize > size)
        // Recreate(
    }

} // namespace Lina::Graphics
