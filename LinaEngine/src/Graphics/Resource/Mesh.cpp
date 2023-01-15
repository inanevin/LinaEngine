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

#include "Graphics/Resource/Mesh.hpp"
#include "Graphics/Core/Backend.hpp"
#include "Math/Color.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/Core/GraphicsCommon.hpp"
#include "Core/Command.hpp"
#include "Log/Log.hpp"
#include "Graphics/Utility/Vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    Mesh::~Mesh()
    {
        m_gpuVtxBuffer.Destroy();
        m_gpuIndexBuffer.Destroy();
        m_vertices.clear();
        m_indices.clear();
    }

    void Mesh::AddVertex(const Vector3& pos, const Vector3& normal, const Vector2& uv)
    {
        m_vertices.emplace_back(Vertex{pos, normal, Color(normal.r, normal.g, normal.b, 1), Vector2(uv.x, 1.0f - uv.y)});
    }

    void Mesh::AddIndices(uint32 i1)
    {
        m_indices.push_back(i1);
    }

    void Mesh::GenerateBuffers()
    {
        if (!RenderEngine::Get()->IsInitialized())
        {
            LINA_ERR("[Mesh] -> Could not create mesh, render engine is not initialized!");
            return;
        }
        const size_t vtxBufferSize = m_vertices.size() * sizeof(Vertex);

        m_cpuVtxBuffer = Buffer{
            .size        = vtxBufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        // Transfer the data from cpu to gpu
        m_cpuVtxBuffer.Create();
        m_cpuVtxBuffer.CopyInto(m_vertices.data(), vtxBufferSize);

        m_gpuVtxBuffer = Buffer{
            .size        = vtxBufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };
        m_gpuVtxBuffer.Create();

        Command vtxCmd;
        vtxCmd.Record = [vtxBufferSize, this](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset      = 0,
                .size              = vtxBufferSize,
            };

            Vector<BufferCopy> regions;
            regions.push_back(copy);

            cmd.CMD_CopyBuffer(m_cpuVtxBuffer._ptr, m_gpuVtxBuffer._ptr, regions);
        };

        vtxCmd.OnSubmitted = [this]() {
            m_gpuVtxBuffer._ready = true;
            m_cpuVtxBuffer.Destroy();
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(vtxCmd);

        // Index buffer
        const size_t indexBufferSize = m_indices.size() * sizeof(uint32);

        m_cpuIndexBuffer = Buffer{
            .size        = indexBufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        // Transfer the data from cpu to gpu
        m_cpuIndexBuffer.Create();
        m_cpuIndexBuffer.CopyInto(m_indices.data(), indexBufferSize);

        m_gpuIndexBuffer = Buffer{
            .size        = indexBufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };
        m_gpuIndexBuffer.Create();

        Command indexCmd;
        indexCmd.Record = [indexBufferSize, this](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset      = 0,
                .size              = indexBufferSize,
            };

            Vector<BufferCopy> regions;
            regions.push_back(copy);

            cmd.CMD_CopyBuffer(m_cpuIndexBuffer._ptr, m_gpuIndexBuffer._ptr, regions);
        };

        indexCmd.OnSubmitted = [this] {
            m_gpuIndexBuffer._ready = true;
            m_cpuIndexBuffer.Destroy();
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(indexCmd);

        m_vtxSize   = static_cast<uint32>(m_vertices.size());
        m_indexSize = static_cast<uint32>(m_indices.size());
        // m_vertices.clear();
        // m_indices.clear();
    }

    void Mesh::ClearInitialBuffers()
    {
        return;
        m_vertices.clear();
        m_indices.clear();
    }
} // namespace Lina::Graphics
