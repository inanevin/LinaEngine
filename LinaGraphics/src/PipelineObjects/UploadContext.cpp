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

#include "PipelineObjects/UploadContext.hpp"
#include "Core/Backend.hpp"
#include "Core/RenderEngine.hpp"
#include "Utility/Command.hpp"

#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    void UploadContext::Create()
    {
        m_fence = Fence{
            .flags = 0,
        };
        m_fence.Create(false);

        m_pool = CommandPool{.familyIndex = Backend::Get()->GetTransferQueue()._family};
        m_pool.Create(false);

        m_buffer = CommandBuffer{.count = 1, .level = CommandBufferLevel::Primary};
        m_buffer.Create(m_pool._ptr);
    }

    void UploadContext::Destroy()
    {
        m_fence.Destroy();
        m_pool.Destroy();
    }

    void UploadContext::SubmitImmediate(Command& cmd)
    {
        LOCK_GUARD(m_mtx);

        // If we don't have a dedicated queue for transfer
        // We're gonna be using the main graphics queue
        // So push it down the list & wait for render engine to query.
        if (!Backend::Get()->SupportsAsyncTransferQueue())
        {
            m_waitingUploads.push(cmd);
            return;
        }

        // If we have a dedicated transfer queue transfer immediately
        Transfer(cmd);
    }

    void UploadContext::Poll()
    {
        if (m_waitingUploads.empty())
            return;

        LOCK_GUARD(m_mtx);
        while (!m_waitingUploads.empty())
        {
            auto& cmd = m_waitingUploads.front();
            Transfer(cmd);
            m_waitingUploads.pop();
        }
    }

    void UploadContext::Transfer(Command& cmd)
    {
        m_buffer.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));
        cmd.Record(m_buffer);
        m_buffer.End();

        if (cmd.OnRecorded)
            cmd.OnRecorded();

        Backend::Get()->GetTransferQueue().Submit(m_fence, m_buffer, 1);

        m_fence.Wait(true, 10.0f);
        m_fence.Reset();
        m_pool.Reset();

        if (cmd.OnSubmitted)
            cmd.OnSubmitted();
    }
} // namespace Lina::Graphics
