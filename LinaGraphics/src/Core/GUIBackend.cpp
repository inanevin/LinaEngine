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

#include "Core/GUIBackend.hpp"
#include "Core/RenderEngine.hpp"
#include "Resource/Material.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include <LinaVG/LinaVG.hpp>
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    GUIBackend* GUIBackend::s_instance = nullptr;

    bool GUIBackend::Initialize()
    {
        s_instance = this;
        Event::EventSystem::Get()->Connect<Event::EPreMainLoop, &GUIBackend::OnPreMainLoop>(this);
        InitializeCategory(m_catDefault);

        return true;
    }

    void GUIBackend::InitializeCategory(LinaVGDrawCategory& cat)
    {

        const size_t vtxSize = sizeof(LinaVG::Vertex) * 10000;
        const size_t indexSize = sizeof(LinaVG::Index) * 4000;

        cat.cpuVtxBuffer = Buffer{
            .size = vtxSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::None,
        };
        cat.cpuVtxBuffer.Create();

        cat.cpuIndexBuffer = Buffer{
            .size = indexSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::None,
        };
        cat.cpuIndexBuffer.Create();

        cat.gpuVtxBuffer = Buffer{
            .size = vtxSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };
        cat.gpuVtxBuffer.Create();

        cat.gpuIndexBuffer = Buffer{
            .size = indexSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };
        cat.gpuIndexBuffer.Create();

        Command vtxCmd;
        vtxCmd.Record = [vtxSize, &cat](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset = 0,
                .size = vtxSize,
            };

            Vector<BufferCopy> regions;
            regions.push_back(copy);
            cmd.CMD_CopyBuffer(cat.cpuVtxBuffer._ptr, cat.gpuVtxBuffer._ptr, regions);
        };

        vtxCmd.OnSubmitted = [&cat]() {
            cat.gpuVtxBuffer._ready = true;
            cat.cpuVtxBuffer.Destroy();
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(vtxCmd);

        Command indexCmd;
        indexCmd.Record = [indexSize, &cat](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset = 0,
                .size = indexSize,
            };

            Vector<BufferCopy> regions;
            regions.push_back(copy);

            cmd.CMD_CopyBuffer(cat.cpuIndexBuffer._ptr, cat.gpuIndexBuffer._ptr, regions);
        };

        indexCmd.OnSubmitted = [&cat] {
            cat.gpuIndexBuffer._ready = true;
            cat.cpuIndexBuffer.Destroy();
        };

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(indexCmd);
    }

    void GUIBackend::Terminate()
    {
        Event::EventSystem::Get()->Disconnect<Event::EPreMainLoop>(this);
    }

    void GUIBackend::StartFrame()
    {

        if (!m_copyVertices.empty())
            vkCmdUpdateBuffer(m_cmd->_ptr, m_gpuVtxBuffer._ptr, 0, m_copyVertices.size() * sizeof(LinaVG::Vertex), m_copyVertices.data());

        if (!m_copyIndices.empty())
            vkCmdUpdateBuffer(m_cmd->_ptr, m_gpuIndexBuffer._ptr, 0, m_copyIndices.size() * sizeof(LinaVG::Index), m_copyIndices.data());

        m_copyVertices.clear();
        m_copyIndices.clear();

        return;

        m_cpuVtxBuffer.CopyInto(m_copyVertices.data(), m_copyVertices.size());
        m_cpuIndexBuffer.CopyInto(m_copyIndices.data(), m_copyIndices.size());

        BufferCopy copyVtx = BufferCopy{
            .destinationOffset = 0,
            .sourceOffset      = 0,
            .size              = m_copyVertices.size(),
        };

        BufferCopy copyIndex = BufferCopy{
            .destinationOffset = 0,
            .sourceOffset      = 0,
            .size              = m_copyIndices.size(),
        };

        Vector<BufferCopy> regionsVtx, regionsIndex;
        regionsVtx.push_back(copyVtx);
        regionsIndex.push_back(copyIndex);

        m_cmd->CMD_CopyBuffer(m_cpuVtxBuffer._ptr, m_gpuVtxBuffer._ptr, regionsVtx);
        m_cmd->CMD_CopyBuffer(m_cpuIndexBuffer._ptr, m_gpuIndexBuffer._ptr, regionsIndex);

        m_copyVertices.clear();
        m_copyIndices.clear();
    }

    void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf)
    {
    }

    void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf)
    {
    }

    void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf)
    {

        LinaVG::DrawBuffer copyBuf;

        for (int i = 0; i < buf->m_vertexBuffer.m_size; i++)
        {
            copyBuf.m_vertexBuffer.push_back(buf->m_vertexBuffer[i]);
            // m_copyVertices.push_back(wat);
        }

        for (int i = 0; i < buf->m_indexBuffer.m_size; i++)
        {
            copyBuf.m_indexBuffer.push_back(buf->m_indexBuffer[i]);
            // m_copyIndices.push_back(buf->m_indexBuffer[i]);
        }
    }

    void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf)
    {
    }
    void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf)
    {
    }

    void GUIBackend::RecordDrawCommands()
    {
        auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);
        mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);

        uint64 offset = 0;
        m_cmd->CMD_BindVertexBuffers(0, 1, m_gpuVtxBuffer._ptr, &offset);
        m_cmd->CMD_BindIndexBuffers(m_gpuIndexBuffer._ptr, 0, IndexType::Uint32);

        uint32 firstIndex   = 0;
        uint32 vertexOffset = 0;
        for (auto& b : m_defaultBuffers)
        {
            m_cmd->CMD_DrawIndexed(b.m_indexBuffer.m_size, 1, firstIndex, vertexOffset, 0);
            firstIndex += b.m_indexBuffer.m_size;
            vertexOffset = sizeof(LinaVG::Vertex) * b.m_vertexBuffer.m_size;
        }
    }

    void GUIBackend::EndFrame()
    {
    }
    void GUIBackend::BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data)
    {
    }
    void GUIBackend::BindFontTexture(LinaVG::BackendHandle texture)
    {
    }
    void GUIBackend::SaveAPIState()
    {
    }
    void GUIBackend::RestoreAPIState()
    {
    }
    LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
    {
        return 0;
    }

    void GUIBackend::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        const Vector2i size = Window::Get()->GetSize();
        const Vector2i pos  = Window::Get()->GetPos();

        Matrix projectionMatrix;

        float       L    = static_cast<float>(pos.x);
        float       R    = static_cast<float>(pos.x + size.x);
        float       T    = static_cast<float>(pos.y);
        float       B    = static_cast<float>(pos.y + size.y);
        const float zoom = 1.0f;

        L *= zoom;
        R *= zoom;
        T *= zoom;
        B *= zoom;

        projectionMatrix[0][0] = 2.0f / (R - L);
        projectionMatrix[0][1] = 0.0f;
        projectionMatrix[0][2] = 0.0f;
        projectionMatrix[0][3] = 0.0f;
        projectionMatrix[1][0] = 0.0f;
        projectionMatrix[1][1] = 2.0f / (T - B);
        projectionMatrix[1][2] = 0.0f;
        projectionMatrix[1][3] = 0.0f;
        projectionMatrix[2][0] = 0.0f;
        projectionMatrix[2][1] = 0.0f;
        projectionMatrix[2][2] = -1.0f;
        projectionMatrix[2][3] = 0.0f;
        projectionMatrix[3][0] = (R + L) / (L - R);
        projectionMatrix[3][1] = (T + B) / (B - T);
        projectionMatrix[3][2] = 0.0f;
        projectionMatrix[3][3] = 1.0f;

        auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);
        mat->SetProperty("projection", projectionMatrix);
        // mat->SetProperty("red", 0.22f);
        // mat->SetProperty("aq", 0.7f);
        // mat->SetProperty("sa", Vector4(0.0f, 0.6f, 0.2f, 1.0f));
    }

} // namespace Lina::Graphics
