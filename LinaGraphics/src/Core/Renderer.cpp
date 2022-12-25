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

#include "Core/Renderer.hpp"
#include "PipelineObjects/Swapchain.hpp"
#include "PipelineObjects/CommandPool.hpp"
#include "Core/RenderData.hpp"
#include "Profiling/Profiler.hpp"

namespace Lina::Graphics
{
    void Renderer::Initialize()
    {
        const uint32 imageSize = static_cast<uint32>(m_swapchain->_images.size());

        for (uint32 i = 0; i < imageSize; i++)
        {
            CommandBuffer buf = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
            m_cmds.push_back(buf);
            m_cmds[i].Create(m_commandPool->_ptr);
        }

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            Semaphore sem;
            m_submitSemaphores.push_back(sem);
            m_submitSemaphores[i].Create();
        }
    }

    void Renderer::Stop()
    {
        m_stopped.store(true);
    }

    bool Renderer::AcquireImage(Frame& frame, uint32 frameIndex)
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        VulkanResult res;
        m_currentImageIndex = m_swapchain->AcquireNextImage(1.0, m_submitSemaphores[frameIndex], res);

        if (m_recreateSwapchain || res == VulkanResult::OutOfDateKHR || res == VulkanResult::SuboptimalKHR)
        {
            if (!m_stopped.load())
                HandleOutOfDateImage();
            return false;
        }
        else if (res != VulkanResult::Success)
        {
            LINA_ASSERT(false, "Could not acquire next image!");
            return false;
        }

        return true;
    }

    void Renderer::BeginCommandBuffer()
    {
        auto& cmd = m_cmds[m_currentImageIndex];
        cmd.Reset(true);
        cmd.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));
    }

    void Renderer::EndCommandBuffer()
    {
        auto& cmd = m_cmds[m_currentImageIndex];
        cmd.End();
    }
} // namespace Lina::Graphics
