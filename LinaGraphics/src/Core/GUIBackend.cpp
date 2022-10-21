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

        const size_t vtxSize   = sizeof(LinaVG::Vertex) * 10000;
        const size_t indexSize = sizeof(LinaVG::Index) * 4000;

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

        return true;
    }

    void GUIBackend::Terminate()
    {
        m_gpuVtxBuffer.Destroy();
        m_gpuIndexBuffer.Destroy();
    }

    void GUIBackend::StartFrame()
    {
        if (m_guiStandard == nullptr)
            m_guiStandard = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);

        const uint32 frame = (RenderEngine::Get()->GetRenderer().GetFrameIndex());

        for (auto& m : m_transientMaterials[frame])
            delete m;

        m_transientMaterials[frame].clear();
    }

    void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf)
    {

        OrderedDrawRequest request;
        request.firstIndex        = m_indexCounter;
        request.vertexOffset      = m_vertexCounter;
        request.type              = LinaVGDrawCategoryType::Gradient;
        request.indexSize         = static_cast<uint32>(buf->m_indexBuffer.m_size);
        request.materialDataIndex = static_cast<uint32>(m_gradientMaterialData.size());

        const uint32 frame = RenderEngine::Get()->GetRenderer().GetFrameIndex();

        request.transientMat = new Material();
        request.transientMat->CreateBuffer();
        request.transientMat->SetShader(m_guiStandard->GetShaderHandle().value);
        request.transientMat->SetProperty("projection", m_projection);
        request.transientMat->SetProperty("color", Vector4(1, 0, 1, 1));
        request.transientMat->CheckUpdatePropertyBuffers();
        m_transientMaterials[frame].push_back(request.transientMat);

        LinaVG::GradientDrawBuffer b;
        b.clipPosX     = buf->clipPosX;
        b.clipPosY     = buf->clipPosY;
        b.clipSizeX    = buf->clipSizeX;
        b.clipSizeY    = buf->clipSizeY;
        b.m_isAABuffer = buf->m_isAABuffer;
        b.m_color      = buf->m_color;
        m_gradientMaterialData.push_back(b);

        for (int i = 0; i < buf->m_vertexBuffer.m_size; i++)
            m_copyVertices.push_back(buf->m_vertexBuffer[i]);

        for (int i = 0; i < buf->m_indexBuffer.m_size; i++)
            m_copyIndices.push_back(buf->m_indexBuffer[i]);

        m_orderedDrawRequests.push_back(request);
        m_indexCounter += static_cast<uint32>(buf->m_indexBuffer.m_size);
        m_vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
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

        const uint32 frame   = RenderEngine::Get()->GetRenderer().GetFrameIndex();
        request.transientMat = new Material();
        request.transientMat->CreateBuffer();
        request.transientMat->SetShader(m_guiStandard->GetShaderHandle().value);
        request.transientMat->SetProperty("projection", m_projection);
        request.transientMat->SetProperty("color", Vector4(1, 0, 0, 1));
        request.transientMat->CheckUpdatePropertyBuffers();
        m_transientMaterials[frame].push_back(request.transientMat);

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
        m_guiStandard->Bind(*m_cmd, RenderPassType::Final, MaterialBindFlag::BindPipeline);

        for (auto& r : m_orderedDrawRequests)
        {
            r.transientMat->Bind(*m_cmd, RenderPassType::Final, MaterialBindFlag::BindDescriptor);

            if (r.type == LinaVGDrawCategoryType::Default)
            {
                // mat->SetProperty("type", 0);
                // mat->CheckUpdatePropertyBuffers();

                // m_cmd->CMD_PushConstants(mat->GetShaderHandle().value->GetPipeline()._layout, GetPipelineStageFlags(PipelineStageFlags::), )
                // mat->SetProperty("type", 1);
                // mat->CheckUpdatePropertyBuffers();
            }
            else if (r.type == LinaVGDrawCategoryType::Textured)
            {
                // auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);
                // mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
            }
            else if (r.type == LinaVGDrawCategoryType::Gradient)
            {

                // mat->SetProperty("type", 1);
                // mat->CheckUpdatePropertyBuffers();

                // auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);

                // auto& matData = m_gradientMaterialData[r.materialDataIndex];
                // mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
                //  mat->SetProperty("type", 0);

                // mat->SetProperty("gradientStart", matData.m_color.start);
                // mat->SetProperty("gradientEnd", matData.m_color.end);
                // mat->SetProperty("gradientType", static_cast<int>(matData.m_color.gradientType));
                // mat->SetProperty("radialSize", matData.m_color.radialSize);
                // mat->SetProperty("isAABuffer", matData.m_isAABuffer);

                //  m_cmd->CMD_PushConstants(mat->GetShaderHandle().value->GetPipeline(RenderPassType::Final)._layout, GetShaderStage(ShaderStage::Fragment), 0, sizeof(pc), &pc);
            }
            else if (r.type == LinaVGDrawCategoryType::SDF)
            {
                // auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIText);
                // mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
            }
            else if (r.type == LinaVGDrawCategoryType::SimpleText)
            {
                // auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIText);
                // mat->BindPipelineAndDescriptors(*m_cmd, RenderPassType::Final);
            }

            m_cmd->CMD_DrawIndexed(r.indexSize, 1, r.firstIndex, r.vertexOffset, 0);
        }

        //  for (auto& r : m_orderedDrawRequests)
        //      delete r.transientMat;

        m_orderedDrawRequests.clear();
        m_defaultMaterialData.clear();
        m_gradientMaterialData.clear();
    }

    void GUIBackend::SyncData()
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

    void GUIBackend::UpdateProjection()
    {
        const Vector2i size = Backend::Get()->GetSwapchain().size;
        const Vector2i pos  = Vector2i();

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

        m_projection[0][0] = 2.0f / (R - L);
        m_projection[0][1] = 0.0f;
        m_projection[0][2] = 0.0f;
        m_projection[0][3] = 0.0f;
        m_projection[1][0] = 0.0f;
        m_projection[1][1] = 2.0f / (T - B);
        m_projection[1][2] = 0.0f;
        m_projection[1][3] = 0.0f;
        m_projection[2][0] = 0.0f;
        m_projection[2][1] = 0.0f;
        m_projection[2][2] = -1.0f;
        m_projection[2][3] = 0.0f;
        m_projection[3][0] = (R + L) / (L - R);
        m_projection[3][1] = (T + B) / (B - T);
        m_projection[3][2] = 0.0f;
        m_projection[3][3] = 1.0f;

        // auto* mat = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);
        // mat->SetProperty("projection", projectionMatrix);
        // mat->CheckUpdatePropertyBuffers();
    }

} // namespace Lina::Graphics
