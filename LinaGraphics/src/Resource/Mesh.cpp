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

#include "Resource/Mesh.hpp"
#include "Core/Backend.hpp"
#include "Math/Color.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/GraphicsCommon.hpp"
#include <vulkan/vulkan.h>
#include "Utility/Vulkan/vk_mem_alloc.h"

namespace Lina::Graphics
{
    Mesh::~Mesh()
    {
        m_gpuVtxBuffer.Destroy();
        m_vertices.clear();
        m_indices.clear();
    }

    void Mesh::AddVertex(const Vector3& pos, const Vector3& normal, const Vector2& uv)
    {
        m_vertices.emplace_back(Vertex{pos, normal, Color(normal.r, normal.g, normal.b, 1), Vector2(uv.x, 1.0f - uv.y)});
    }

    void Mesh::AddIndices(uint32 i1, uint32 i2, uint32 i3)
    {
        m_indices.push_back(Vector3ui(i1, i2, i3));
    }

    void Mesh::GenerateBuffers()
    {
        if (!RenderEngine::Get()->IsInitialized())
        {
            LINA_ERR("[Mesh] -> Could not create mesh, render engine is not initialized!");
            return;
        }

        const size_t bufferSize = m_vertices.size() * sizeof(Vertex);

        Buffer cpuBuffer = Buffer{
            .size        = bufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        // Transfer the data from cpu to gpu
        cpuBuffer.Create();
        cpuBuffer.CopyInto(m_vertices.data(), bufferSize);

        m_gpuVtxBuffer = Buffer{
            .size        = bufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };
        m_gpuVtxBuffer.Create();

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(
            [=](CommandBuffer& cmd) {
                BufferCopy copy = BufferCopy{
                    .destinationOffset = 0,
                    .sourceOffset      = 0,
                    .size              = bufferSize,
                };

                Vector<BufferCopy> regions;
                regions.push_back(copy);

                cmd.CMD_CopyBuffer(cpuBuffer._ptr, m_gpuVtxBuffer._ptr, regions);
            });

        cpuBuffer.Destroy();
    }
} // namespace Lina::Graphics
