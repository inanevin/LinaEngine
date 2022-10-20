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

        const size_t vtxSize   = sizeof(LinaVG::Vertex) * 10000;
        const size_t indexSize = sizeof(LinaVG::Index) * 4000;

        m_cpuVtxBuffer = Buffer{
            .size          = vtxSize,
            .bufferUsage   = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage   = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::None,
        };
        m_cpuVtxBuffer.Create();

        m_cpuIndexBuffer = Buffer{
            .size          = indexSize,
            .bufferUsage   = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage   = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::None,
        };
        m_cpuIndexBuffer.Create();

        m_gpuVtxBuffer = Buffer{
            .size        = vtxSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };
        m_gpuVtxBuffer.Create();

        m_gpuIndexBuffer = Buffer{
            .size        = indexSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
            .memoryUsage = MemoryUsageFlags::GpuOnly,
        };
        m_gpuIndexBuffer.Create();

        Command vtxCmd;
        vtxCmd.Record = [vtxSize, this](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset      = 0,
                .size              = vtxSize,
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

        Command indexCmd;
        indexCmd.Record = [indexSize, this](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset      = 0,
                .size              = indexSize,
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

        return true;
    }

    void GUIBackend::Terminate()
    {
        Event::EventSystem::Get()->Disconnect<Event::EPreMainLoop>(this);
        m_gpuVtxBuffer.Destroy();
        m_gpuIndexBuffer.Destroy();
    }

    void GUIBackend::StartFrame()
    {
    }

    void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf)
    {
    }

    void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf)
    {
    }

    void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf)
    {
        OrderedDrawRequest request;
        request.firstIndex        = m_indexCounter;
        request.vertexOffset      = m_vertexCounter;
        request.type              = LinaVGDrawCategoryType::Default;
        request.indexSize         = static_cast<uint32>(buf->m_indexBuffer.m_size);
        request.materialDataIndex = static_cast<uint32>(m_defaultMaterialData.size());

        LinaVG::DrawBuffer b;
        b.clipPosX  = buf->clipPosX;
        b.clipPosY  = buf->clipPosY;
        b.clipSizeX = buf->clipSizeX;
        b.clipSizeY = buf->clipSizeY;
        m_defaultMaterialData.push_back(b);

        for (int i = 0; i < buf->m_vertexBuffer.m_size; i++)
            m_copyVertices.push_back(buf->m_vertexBuffer[i]);

        for (int i = 0; i < buf->m_indexBuffer.m_size; i++)
            m_copyIndices.push_back(buf->m_indexBuffer[i]);

        m_orderedDrawRequests.push_back(request);
        m_indexCounter += static_cast<uint32>(buf->m_indexBuffer.m_size);
        m_vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
    }

    void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf)
    {
    }
    void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf)
    {
    }

    void GUIBackend::EndFrame()
    {
        if (!m_copyVertices.empty())
            vkCmdUpdateBuffer(m_cmd->_ptr, m_gpuVtxBuffer._ptr, 0, m_copyVertices.size() * sizeof(LinaVG::Vertex), m_copyVertices.data());

        if (!m_copyIndices.empty())
            vkCmdUpdateBuffer(m_cmd->_ptr, m_gpuIndexBuffer._ptr, 0, m_copyIndices.size() * sizeof(LinaVG::Index), m_copyIndices.data());

        m_copyVertices.clear();
        m_copyIndices.clear();

        m_indexCounter = m_vertexCounter = 0;
    }

    void GUIBackend::RecordDrawCommands()
    {
        uint64 offset = 0;
        m_cmd->CMD_BindVertexBuffers(0, 1, m_gpuVtxBuffer._ptr, &offset);
        m_cmd->CMD_BindIndexBuffers(m_gpuIndexBuffer._ptr, 0, IndexType::Uint32);

        for (auto& r : m_orderedDrawRequests)
        {
            if (r.type == LinaVGDrawCategoryType::Default)
            {
                auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);
                mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
            }
            else if (r.type == LinaVGDrawCategoryType::Textured)
            {
                auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);
                mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
            }
            else if (r.type == LinaVGDrawCategoryType::Gradient)
            {
                auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);
                mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
            }
            else if (r.type == LinaVGDrawCategoryType::SDF)
            {
                auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIText);
                mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
            }
            else if (r.type == LinaVGDrawCategoryType::SimpleText)
            {
                auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIText);
                mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
            }

            m_cmd->CMD_DrawIndexed(r.indexSize, 1, r.firstIndex, r.vertexOffset, 0);
        }

        m_orderedDrawRequests.clear();
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
        float       T    = static_cast<float>(pos.y + size.y);
        float       B    = static_cast<float>(pos.y);
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
