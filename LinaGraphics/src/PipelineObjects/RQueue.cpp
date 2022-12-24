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

#include "PipelineObjects/RQueue.hpp"
#include "Data/Vector.hpp"
#include "Core/Backend.hpp"
#include "PipelineObjects/Fence.hpp"
#include "PipelineObjects/Semaphore.hpp"
#include "PipelineObjects/CommandBuffer.hpp"
#include "Log/Log.hpp"
#include "vulkan/vulkan.h"

namespace Lina::Graphics
{
    void RQueue::Get(uint32 family, uint32 index)
    {
        _family = family;
        vkGetDeviceQueue(Backend::Get()->GetDevice(), _family, index, &_ptr);
        LINA_ASSERT(_ptr, "[Render Queue] -> Could not get device queue.");
    }

    void RQueue::Submit(const Vector<Semaphore>& waitSemaphores, const Vector<Semaphore>& signalSemaphores, const Fence& fence, const CommandBuffer& cmd, uint32 submitCount)
    {
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        Vector<VkSemaphore>          _waitSemaphores;
        Vector<VkSemaphore>          _signalSemaphores;
        Vector<VkPipelineStageFlags> _stageFlags;

        for (auto& s : waitSemaphores)
        {
            _waitSemaphores.push_back(s._ptr);
            _stageFlags.push_back(waitStage);
        }

        for (auto& s : signalSemaphores)
            _signalSemaphores.push_back(s._ptr);

        VkSubmitInfo info = VkSubmitInfo{
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = nullptr,
            .waitSemaphoreCount   = static_cast<uint32>(_waitSemaphores.size()),
            .pWaitSemaphores      = &_waitSemaphores[0],
            .pWaitDstStageMask    = &_stageFlags[0],
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmd._ptr,
            .signalSemaphoreCount = static_cast<uint32>(_signalSemaphores.size()),
            .pSignalSemaphores    = &_signalSemaphores[0],
        };
        VkResult result = vkQueueSubmit(_ptr, submitCount, &info, fence._ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Render Queue] -> Failed submitting to queue!");
    }

    void RQueue::Submit(const Semaphore& semaphore, const Semaphore& signalSemaphore, const Fence& fence, const CommandBuffer& cmd, uint32 submitCount)
    {
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo info = VkSubmitInfo{
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = nullptr,
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = &semaphore._ptr,
            .pWaitDstStageMask    = &waitStage,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmd._ptr,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &signalSemaphore._ptr,
        };
        VkResult result = vkQueueSubmit(_ptr, submitCount, &info, fence._ptr);
        LINA_ASSERT(result == VK_SUCCESS, "[Render Queue] -> Failed submitting to queue!");
    }

    void RQueue::Submit(const Fence& fence, const CommandBuffer& cmd, uint32 submitCount)
    {
        VkSubmitInfo info = VkSubmitInfo{
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = nullptr,
            .waitSemaphoreCount   = 0,
            .pWaitSemaphores      = nullptr,
            .pWaitDstStageMask    = nullptr,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmd._ptr,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores    = nullptr,
        };
        VkResult result = vkQueueSubmit(_ptr, submitCount, &info, fence._ptr);

        LINA_ASSERT(result == VK_SUCCESS, "[Render Queue] -> Failed submitting to queue!");
    }

    void RQueue::Present(const Semaphore& waitSemaphore, uint32 swapchainImageIndex)
    {
        auto&        swapchain = Backend::Get()->GetMainSwapchain();
        const uint32 index     = swapchainImageIndex;

        VkPresentInfoKHR info = VkPresentInfoKHR{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext              = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &waitSemaphore._ptr,
            .swapchainCount     = static_cast<uint32>(1),
            .pSwapchains        = &swapchain._ptr,
            .pImageIndices      = &index,
        };
        VkResult result = vkQueuePresentKHR(_ptr, &info);
        LINA_ASSERT(result == VK_SUCCESS, "[Render Queue] -> Failed presenting image from queue!");
    }

    void RQueue::Present(const Vector<Semaphore>& waitSemaphores, const Vector<Swapchain>& swapchains, const Vector<uint32>& indices)
    {
        auto& swapchain = Backend::Get()->GetMainSwapchain();

        Vector<VkSwapchainKHR> _swapchains;
        Vector<VkSemaphore>    _waitSemaphores;

        for (auto& swp : swapchains)
            _swapchains.push_back(swp._ptr);

        for (auto& sm : waitSemaphores)
            _waitSemaphores.push_back(sm._ptr);

        VkPresentInfoKHR info = VkPresentInfoKHR{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext              = nullptr,
            .waitSemaphoreCount = static_cast<uint32>(_waitSemaphores.size()),
            .pWaitSemaphores    = &_waitSemaphores[0],
            .swapchainCount     = static_cast<uint32>(_swapchains.size()),
            .pSwapchains        = &_swapchains[0],
            .pImageIndices      = &indices[0],
        };
        VkResult result = vkQueuePresentKHR(_ptr, &info);
        LINA_ASSERT(result == VK_SUCCESS, "[Render Queue] -> Failed presenting image from queue!");
    }

    void RQueue::WaitIdle()
    {
        vkQueueWaitIdle(_ptr);
    }
} // namespace Lina::Graphics
