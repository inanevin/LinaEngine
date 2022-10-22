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
#include "Platform/LinaVGIncl.hpp"
#include "Resource/Texture.hpp"
#include "Utility/Vulkan/VulkanUtility.hpp"
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
        for (auto& pair : m_transientMaterials)
        {
            for (auto& m : pair.second)
                delete m;

            pair.second.clear();
        }

        for (auto& t : m_fontTextures)
        {
            t.second->m_cpuBuffer.Destroy();
            delete t.second;
        }

        m_transientMaterials.clear();
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

        const uint32 frame   = RenderEngine::Get()->GetRenderer().GetFrameIndex();
        request.transientMat = new Material();
        request.transientMat->CreateBuffer();
        request.transientMat->SetShader(m_guiStandard->GetShaderHandle().value);
        request.transientMat->SetProperty("projection", m_projection);
        request.transientMat->SetProperty("type", 1);
        request.transientMat->SetProperty("gradientStart", buf->m_color.start);
        request.transientMat->SetProperty("gradientEnd", buf->m_color.end);
        request.transientMat->SetProperty("gradientType", static_cast<int>(buf->m_color.gradientType));
        request.transientMat->SetProperty("radialSize", buf->m_color.radialSize);
        request.transientMat->SetProperty("isAABuffer", buf->m_isAABuffer);
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
        OrderedDrawRequest request;
        request.firstIndex        = m_indexCounter;
        request.vertexOffset      = m_vertexCounter;
        request.type              = LinaVGDrawCategoryType::Default;
        request.indexSize         = static_cast<uint32>(buf->m_indexBuffer.m_size);
        request.materialDataIndex = static_cast<uint32>(m_defaultMaterialData.size());

        Texture*     txt     = Resources::ResourceManager::Get()->GetResource<Texture>(buf->m_textureHandle);
        const uint32 frame   = RenderEngine::Get()->GetRenderer().GetFrameIndex();
        request.transientMat = new Material();
        request.transientMat->CreateBuffer();
        request.transientMat->SetShader(m_guiStandard->GetShaderHandle().value);
        request.transientMat->SetProperty("projection", m_projection);
        request.transientMat->SetProperty("type", 2);
        request.transientMat->SetProperty("tiling", Vector2(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y));
        request.transientMat->SetProperty("offset", Vector2(buf->m_textureUVOffset.x, buf->m_textureUVOffset.y));
        request.transientMat->SetTexture("diffuse", m_fontTextures[0]);
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
        request.transientMat->SetProperty("type", 0);
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

            m_cmd->CMD_DrawIndexed(r.indexSize, 1, r.firstIndex, r.vertexOffset, 0);
        }

        m_orderedDrawRequests.clear();
        m_defaultMaterialData.clear();
        m_gradientMaterialData.clear();
    }

    void GUIBackend::SyncData()
    {
    }

    size_t                        bufSize = 0;
    Vector<Vector<unsigned char>> pixels;

    uint32 previousEnd  = 0;
    uint32 previousEndX = 0;
    uint32 previousEndY = 0;
    uint32 prevSize     = 0;

    Vector<Command> cmds;
    static uint32   biggest = 0;

    void GUIBackend::RecordCopyCommand(Texture* txt, uint32 width, uint32 height, uint32 offset, int32 offsetX, int32 offsetY)
    {
    }

    void GUIBackend::BufferFontTextureAtlas(int width, int height, int offsetX, int offsetY, unsigned char* data)
    {
        Texture*     txt         = m_fontTextures[m_bufferingFontTexture];
        const uint32 bufSize     = width * height;
        uint32       startOffset = offsetY * txt->m_extent.width + offsetX;

        for (int i = 0; i < height; i++)
        {
            const uint32 size = width;
            txt->m_cpuBuffer.CopyIntoPadded(&data[width * i], size, startOffset);
            startOffset += txt->m_extent.width;
        }
    }

    void GUIBackend::UploadFontTexture(uint32 handle)
    {
        Texture* txt       = m_fontTextures[handle];
        Offset3D imgOffset = Offset3D{.x = 0, .y = 0, .z = 0};
        txt->WriteToGPUImage(0, nullptr, 0, imgOffset, txt->m_extent, false);
    }

    void GUIBackend::BindFontTexture(LinaVG::BackendHandle texture)
    {
        m_bufferingFontTexture = texture;
    }

    void GUIBackend::SaveAPIState()
    {
    }

    void GUIBackend::RestoreAPIState()
    {
    }

    LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
    {
        Extent3D ext = Extent3D{
            .width  = static_cast<uint32>(width),
            .height = static_cast<uint32>(height),
            .depth  = 1,
        };

        const uint32 bufferSize = ext.width * ext.height;
        bufSize                 = bufferSize;
        pixels.resize(ext.height);

        for (auto& p : pixels)
            p.resize(ext.width);

        Texture* txt  = new Texture();
        txt->m_extent = ext;

        // Cpu buf
        txt->m_cpuBuffer = Buffer{
            .size        = bufferSize,
            .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
            .memoryUsage = MemoryUsageFlags::CpuOnly,
        };

        txt->m_cpuBuffer.Create();

        ImageSubresourceRange range;
        range.aspectFlags   = GetImageAspectFlags(ImageAspectFlags::AspectColor);
        const Format format = Format::R8_UNORM;

        // Gpu buf
        txt->m_gpuImage = Image{
            .format          = format,
            .tiling          = ImageTiling::Linear,
            .extent          = ext,
            .imageUsageFlags = GetImageUsage(ImageUsageFlags::Sampled) | GetImageUsage(ImageUsageFlags::TransferDest),
            .subresRange     = range,
        };

        txt->m_gpuImage.Create(true, false);

        // Sampler
        txt->m_sampler = Sampler{
            .minFilter     = Filter::Linear,
            .magFilter     = Filter::Linear,
            .u             = SamplerAddressMode::ClampToEdge,
            .v             = SamplerAddressMode::ClampToEdge,
            .w             = SamplerAddressMode::ClampToEdge,
            .mipLodBias    = 0.0f,
            .maxAnisotropy = 1.0f,
            .minLod        = 0.0f,
            .maxLod        = 1.0f,
            .borderColor   = BorderColor::FloatOpaqueWhite,
        };
        txt->m_sampler.Create(false);

        const uint32 handle    = static_cast<uint32>(m_fontTextures.size());
        m_fontTextures[handle] = txt;
        return handle;
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
