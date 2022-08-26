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

#include "PipelineObjects/CommandPool.hpp"
#include "Core/Backend.hpp"
#include "Core/RenderEngine.hpp"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    void CommandPool::Create(bool destroyAuto)
    {
        VkCommandPoolCreateInfo commandPoolInfo = VkCommandPoolCreateInfo{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = flags,
            .queueFamilyIndex = familyIndex,
        };

        VkResult result = vkCreateCommandPool(Backend::Get()->GetDevice(), &commandPoolInfo, Backend::Get()->GetAllocator(), &_ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Command Pool] -> Could not create command pool!");

        if (destroyAuto)
        {
            VkCommandPool_T* ptr = _ptr;
            RenderEngine::Get()->GetMainDeletionQueue().Push(std::bind([ptr]() {
                vkDestroyCommandPool(Backend::Get()->GetDevice(), ptr, Backend::Get()->GetAllocator());
            }));
        }
    }

    void CommandPool::Destroy()
    {
        vkDestroyCommandPool(Backend::Get()->GetDevice(), _ptr, Backend::Get()->GetAllocator());
    }

    void CommandPool::Reset()
    {
        vkResetCommandPool(Backend::Get()->GetDevice(), _ptr, 0);
    }

} // namespace Lina::Graphics
