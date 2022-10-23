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
#include "Utility/Vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
    GUIBackend* GUIBackend::s_instance = nullptr;

#define BUFFER_LIMIT        50000
#define MATERIAL_POOL_COUNT 30

    bool GUIBackend::Initialize()
    {
        s_instance = this;
        Event::EventSystem::Get()->Connect<Event::EPreMainLoop, &GUIBackend::OnPreMainLoop>(this);
        CreateBufferCapsule();
        CreateBufferCapsule();
        return true;
    }

    void GUIBackend::CreateBufferCapsule()
    {
        m_bufferCapsules.push_back(BufferCapsule());
        auto& caps = m_bufferCapsules[m_bufferCapsules.size() - 1];

        //  caps.cpuVtxBuffer = Buffer{
        //      .size        = BUFFER_LIMIT,
        //      .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
        //      .memoryUsage = MemoryUsageFlags::CpuOnly,
        //  };
        //
        //  caps.cpuIndexBuffer = Buffer{
        //      .size        = BUFFER_LIMIT,
        //      .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferSrc),
        //      .memoryUsage = MemoryUsageFlags::CpuOnly,
        //  };
        //
        //  caps.gpuVtxBuffer = Buffer{
        //      .size        = BUFFER_LIMIT,
        //      .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
        //      .memoryUsage = MemoryUsageFlags::GpuOnly,
        //  };
        //
        //  caps.gpuIndexBuffer = Buffer{
        //      .size        = BUFFER_LIMIT,
        //      .bufferUsage = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer) | GetBufferUsageFlags(BufferUsageFlags::TransferDst),
        //      .memoryUsage = MemoryUsageFlags::GpuOnly,
        //  };
        //
        //  caps.cpuIndexBuffer.Create();
        //  caps.cpuVtxBuffer.Create();
        //  caps.gpuVtxBuffer.Create();
        //  caps.gpuIndexBuffer.Create();

        caps.testVtxBuffer = Buffer{
            .size          = BUFFER_LIMIT,
            .bufferUsage   = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer),
            .memoryUsage   = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::HostVisible,
        };

        caps.testIndexBuffer = Buffer{
            .size          = BUFFER_LIMIT,
            .bufferUsage   = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer),
            .memoryUsage   = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::HostVisible,
        };



        caps.testIndexBuffer.Create();
        caps.testVtxBuffer.Create();

        //  caps.indexPtr = caps.testIndexBuffer.MapBuffer();
        //  caps.vtxPtr   = caps.testVtxBuffer.MapBuffer();
    }

    void GUIBackend::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        if (m_guiStandard == nullptr)
            m_guiStandard = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pool = m_materialPools[i];
            pool.materials.resize(MATERIAL_POOL_COUNT, Material());
            pool.index = 0;

            int a = 0;
            for (auto& m : pool.materials)
            {
                m.CreateBuffer();
                m.SetShader(m_guiStandard->GetShaderHandle().value);
            }
        }
    }

    void GUIBackend::Terminate()
    {
        Event::EventSystem::Get()->Disconnect<Event::EPreMainLoop>(this);

        for (auto& t : m_fontTextures)
        {
            t.second->m_cpuBuffer.Destroy();
            delete t.second;
        }

        for (auto& b : m_bufferCapsules)
        {
            b.cpuIndexBuffer.Destroy();
            b.cpuVtxBuffer.Destroy();
            b.gpuVtxBuffer.Destroy();
            b.gpuIndexBuffer.Destroy();
            // b.testIndexBuffer.UnmapBuffer();
            // b.testVtxBuffer.UnmapBuffer();
        }
    }

    void GUIBackend::StartFrame()
    {
        const uint32 frame           = RenderEngine::Get()->GetRenderer().GetFrameIndex();
        m_materialPools[frame].index = 0;
    }

    void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Gradient);
        mat->SetProperty("intvar1", 1);
        mat->SetProperty("color1", buf->m_color.start);
        mat->SetProperty("color2", buf->m_color.end);
        mat->SetProperty("intvar2", static_cast<int>(buf->m_color.gradientType));
        mat->SetProperty("floatvar1", buf->m_color.radialSize);
        mat->SetProperty("intvar3", buf->m_isAABuffer);
        mat->CheckUpdatePropertyBuffers();

        const uint32 frame = RenderEngine::Get()->GetRenderer().GetFrameIndex();
    }

    void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Textured);
        Texture*  txt = Resources::ResourceManager::Get()->GetResource<Texture>(buf->m_textureHandle);
        mat->SetProperty("intvar1", 2);
        mat->SetProperty("vec2pack1", Vector2(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y));
        mat->SetProperty("vec2pack2", Vector2(buf->m_textureUVOffset.x, buf->m_textureUVOffset.y));
        mat->SetTexture("diffuse", txt);
        mat->CheckUpdatePropertyBuffers();
    }

    void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Default);
        mat->SetProperty("intvar1", 0);
        mat->CheckUpdatePropertyBuffers();
    }

    void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::SimpleText);
        mat->SetProperty("intvar1", 3);
        mat->SetTexture("diffuse", m_fontTextures[buf->m_textureHandle]);
        mat->CheckUpdatePropertyBuffers();
    }

    void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf)
    {
        Material*   mat              = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::SDF);
        const float thickness        = 1.0f - Math::Clamp(buf->m_thickness, 0.0f, 1.0f);
        const float softness         = Math::Clamp(buf->m_softness, 0.0f, 10.0f) * 0.1f;
        const float outlineThickness = Math::Clamp(buf->m_outlineThickness, 0.0f, 1.0f);
        mat->SetProperty("intvar1", 4);
        mat->SetProperty("color1", buf->m_outlineColor);
        mat->SetProperty("floatvar3", outlineThickness);
        mat->SetProperty("floatvar2", softness);
        mat->SetProperty("floatvar1", thickness);
        mat->SetProperty("intvar3", buf->m_flipAlpha);
        mat->SetProperty("intvar2", buf->m_outlineThickness != 0.0f ? 1 : 0);
        mat->SetTexture("diffuse", m_fontTextures[buf->m_textureHandle]);
        mat->CheckUpdatePropertyBuffers();
    }

    Material* GUIBackend::AddOrderedDrawRequest(LinaVG::DrawBuffer* buf, LinaVGDrawCategoryType type)
    {
        const uint32 frame         = RenderEngine::Get()->GetRenderer().GetFrameIndex();
        auto&        targetCapsule = m_bufferCapsules[frame];

        OrderedDrawRequest request;
        request.firstIndex   = targetCapsule.indexCounter;
        request.vertexOffset = targetCapsule.vertexCounter;
        request.type         = type;
        request.indexSize    = static_cast<uint32>(buf->m_indexBuffer.m_size);
        request.meta.clipX   = buf->clipPosX;
        request.meta.clipY   = buf->clipPosY;
        request.meta.clipW   = buf->clipSizeX;
        request.meta.clipH   = buf->clipSizeY;

        auto& pool = m_materialPools[frame];

        uint32 matId = pool.index;

        if (matId >= pool.materials.size())
        {
            LINA_ASSERT(false, "[GUI Backend] -> Insufficient material pool size!");
        }
        else
            pool.index++;

        request.transientMat = &pool.materials[matId];
        request.transientMat->SetProperty("projection", m_projection);

        targetCapsule.orderedDrawRequests.push_back(request);

        // targetCapsule.cpuVtxBuffer.CopyIntoPadded(buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex), targetCapsule.vertexCounter * sizeof(LinaVG::Vertex));
        // targetCapsule.cpuIndexBuffer.CopyIntoPadded(buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index), targetCapsule.indexCounter * sizeof(LinaVG::Index));
         targetCapsule.testVtxBuffer.CopyIntoPadded(buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex), targetCapsule.vertexCounter * sizeof(LinaVG::Vertex));
         targetCapsule.testIndexBuffer.CopyIntoPadded(buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index), targetCapsule.indexCounter * sizeof(LinaVG::Index));
     
        targetCapsule.indexCounter += static_cast<uint32>(buf->m_indexBuffer.m_size);
        targetCapsule.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
        return request.transientMat;
    }

    int  counter = 0;
    void GUIBackend::EndFrame()
    {
        const uint32 frame         = RenderEngine::Get()->GetRenderer().GetFrameIndex();
        auto&        b             = m_bufferCapsules[frame];
        const size_t vtxBufferSize = b.cpuVtxBuffer.size;
        b.indexCounter = b.vertexCounter = 0;

        return;

        counter++;
        if (b.gpuVtxBuffer.size < vtxBufferSize)
        {
            b.gpuVtxBuffer.Destroy();
            b.gpuVtxBuffer.size = vtxBufferSize;
            b.gpuVtxBuffer.Create();
        }

        Command vtxCmd;
        vtxCmd.Record = [vtxBufferSize, this, &b](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset      = 0,
                .size              = vtxBufferSize,
            };

            Vector<BufferCopy> regions;
            regions.push_back(copy);

            cmd.CMD_CopyBuffer(b.cpuVtxBuffer._ptr, b.gpuVtxBuffer._ptr, regions);
        };

        vtxCmd.OnSubmitted = [this]() {};

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(vtxCmd);

        // Index buffer
        const size_t indexBufferSize = b.cpuIndexBuffer.size;

        if (b.gpuIndexBuffer.size < indexBufferSize)
        {
            b.gpuIndexBuffer.Destroy();
            b.gpuIndexBuffer.size = indexBufferSize;
            b.gpuIndexBuffer.Create();
        }
        Command indexCmd;
        indexCmd.Record = [indexBufferSize, this, &b](CommandBuffer& cmd) {
            BufferCopy copy = BufferCopy{
                .destinationOffset = 0,
                .sourceOffset      = 0,
                .size              = indexBufferSize,
            };

            Vector<BufferCopy> regions;
            regions.push_back(copy);

            cmd.CMD_CopyBuffer(b.cpuIndexBuffer._ptr, b.gpuIndexBuffer._ptr, regions);
        };
        indexCmd.OnSubmitted = [this] {};

        RenderEngine::Get()->GetGPUUploader().SubmitImmediate(indexCmd);
    }

    void GUIBackend::RecordDrawCommands()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        const uint32 frame = RenderEngine::Get()->GetRenderer().GetFrameIndex();

        auto& b = m_bufferCapsules[frame];

        if (b.orderedDrawRequests.empty())
            return;

        uint64 offset = 0;
        m_cmd->CMD_BindVertexBuffers(0, 1, b.testVtxBuffer._ptr, &offset);
        m_cmd->CMD_BindIndexBuffers(b.testIndexBuffer._ptr, 0, IndexType::Uint32);
        m_guiStandard->Bind(*m_cmd, RenderPassType::Final, MaterialBindFlag::BindPipeline);

        for (auto& r : b.orderedDrawRequests)
        {
            r.transientMat->Bind(*m_cmd, RenderPassType::Final, MaterialBindFlag::BindDescriptor);
            m_cmd->CMD_DrawIndexed(r.indexSize, 1, r.firstIndex, r.vertexOffset, 0);
        }

        b.orderedDrawRequests.clear();
    }

    LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
    {
        Extent3D ext = Extent3D{
            .width  = static_cast<uint32>(width),
            .height = static_cast<uint32>(height),
            .depth  = 1,
        };

        const uint32 bufferSize = ext.width * ext.height;
        Texture*     txt        = new Texture();
        txt->m_extent           = ext;

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

        // Skipping 0 index
        const uint32 handle    = static_cast<uint32>(m_fontTextures.size() + 1);
        m_fontTextures[handle] = txt;
        m_bufferingFontTexture = handle;
        return handle;
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
    }

    void GUIBackend::SaveAPIState()
    {
    }

    void GUIBackend::RestoreAPIState()
    {
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
    }

} // namespace Lina::Graphics
