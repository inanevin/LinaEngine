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

#ifndef UploadContext_HPP
#define UploadContext_HPP

#include "Graphics/Core/GraphicsCommon.hpp"
#include "Data/Queue.hpp"
#include "Data/Mutex.hpp"
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"
#include "Fence.hpp"
#include "Core/Command.hpp"
#include <functional>

namespace Lina::Graphics
{
    class RQueue;
    class DeletionQueue;
    
    class UploadContext
    {
    public:
        void SubmitImmediate(Command& cmd);

    private:
        friend class RenderEngine;

        void Create(DeletionQueue& deletionQueue);
        void Destroy();
        void Poll();
        void Transfer(Command& cmd);

    private:
        Mutex          m_mtx;
        Fence          m_fence;
        CommandPool    m_pool;
        CommandBuffer  m_cmd;
        Queue<Command> m_waitingUploads;
    };
} // namespace Lina::Graphics

#endif
