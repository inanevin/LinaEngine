/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Rendering/UniformBuffer.hpp"

#include "Core/RenderEngineBackend.hpp"

namespace Lina::Graphics
{
    UniformBuffer::~UniformBuffer()
    {
        if (m_isConstructed)
            m_engineBoundID = m_renderDevice->ReleaseUniformBuffer(m_engineBoundID);
    }

    // Updates the uniform buffer w/ new data, allows dynamic size change.
    void UniformBuffer::Construct(uintptr dataSize, BufferUsage usage, const void* data)
    {
        m_renderDevice  = RenderEngineBackend::Get()->GetRenderDevice();
        m_bufferSize    = dataSize;
        m_engineBoundID = m_renderDevice->CreateUniformBuffer(data, dataSize, usage);
        m_isConstructed = true;
    }

    void UniformBuffer::Bind(uint32 point)
    {
        m_renderDevice->BindUniformBuffer(m_engineBoundID, point);
    }

    void UniformBuffer::Update(const void* data, uintptr offset, uintptr dataSize)
    {
        m_renderDevice->UpdateUniformBuffer(m_engineBoundID, data, offset, dataSize);
    }

    void UniformBuffer::Update(const void* data, uintptr dataSize)
    {
        m_renderDevice->UpdateUniformBuffer(m_engineBoundID, data, dataSize);
    }

} // namespace Lina::Graphics
