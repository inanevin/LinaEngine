/*
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

#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/Resource/Material.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/Screen.hpp"
#include "Profiling/Profiler.hpp"
#include "Math/Math.hpp"
#include "Graphics/Core/RenderData.hpp"
#include "Graphics/Utility/Vulkan/VulkanUtility.hpp"
#include "Graphics/Utility/Vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace Lina::Graphics
{
#define BUFFER_LIMIT        50000
#define MATERIAL_POOL_COUNT 120
#define TO_LINA_VEC4(V)     Vector4(V.x, V.y, V.z, V.w)

    bool GUIBackend::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::EPreMainLoop, &GUIBackend::OnPreMainLoop>(this);
        return true;
    }

    void GUIBackend::CreateBufferCapsule(bool setMaterials)
    {
        BufferCapsule capsule;

        capsule.vtxBuffer = Buffer{
            .size          = BUFFER_LIMIT,
            .bufferUsage   = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer),
            .memoryUsage   = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::HostVisible,
        };

        capsule.indxBuffer = Buffer{
            .size          = BUFFER_LIMIT,
            .bufferUsage   = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer),
            .memoryUsage   = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::HostVisible,
        };

        capsule.indxBuffer.Create();
        capsule.vtxBuffer.Create();

        if (setMaterials)
        {
            for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                MaterialPool pool;

                pool.materials.resize(MATERIAL_POOL_COUNT, nullptr);
                pool.index = 0;

                for (auto& m : pool.materials)
                {
                    m = new Material();
                    m->CreateBuffer();
                    m->SetShader(m_guiStandard->GetShaderHandle().value);
                }

                capsule.pools.push_back(pool);
            }
        }

        m_bufferCapsules.push_back(capsule);
    }

    void GUIBackend::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        if (m_guiStandard == nullptr)
            m_guiStandard = m_renderEngine->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);

        for (int i = 0; i < 1; i++)
            CreateBufferCapsule(true);
    }

    void GUIBackend::Terminate()
    {
        Event::EventSystem::Get()->Disconnect<Event::EPreMainLoop>(this);

        for (auto& t : m_fontTextures)
        {
            t.second->m_cpuBuffer.Destroy();
            delete t.second;
        }

        for (auto& capsule : m_bufferCapsules)
        {
            capsule.indxBuffer.Destroy();
            capsule.vtxBuffer.Destroy();

            for (auto& pool : capsule.pools)
            {
                for (auto material : pool.materials)
                    delete material;
            }
        }
    }

    void GUIBackend::StartFrame(int threadCount)
    {
        while (static_cast<int>(m_bufferCapsules.size()) < threadCount)
            CreateBufferCapsule(true);
    }

    void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf, int thread)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Gradient, thread);
        mat->SetProperty("intvar1", 1);
        mat->SetProperty("color1", TO_LINA_VEC4(buf->m_color.start));
        mat->SetProperty("color2", TO_LINA_VEC4(buf->m_color.end));
        mat->SetProperty("intvar2", static_cast<int>(buf->m_color.gradientType));
        mat->SetProperty("floatvar1", buf->m_color.radialSize);
        mat->SetProperty("intvar3", buf->m_isAABuffer);
        mat->CheckUpdatePropertyBuffers();
    }

    void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf, int thread)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Textured, thread);
        Texture*  txt = Resources::ResourceManager::Get()->GetResource<Texture>(buf->m_textureHandle);

        mat->SetProperty("intvar1", 2);
        mat->SetProperty("vec2pack1", Vector2(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y));
        mat->SetProperty("vec2pack2", Vector2(buf->m_textureUVOffset.x, buf->m_textureUVOffset.y));
        mat->SetProperty("color1", Vector4(buf->m_tint.x, buf->m_tint.y, buf->m_tint.z, buf->m_tint.w));
        mat->SetTexture("diffuse", txt);
        mat->CheckUpdatePropertyBuffers();
    }

    void GUIBackend::DrawDefault(LinaVG::DrawBuffer* buf, int thread)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Default, thread);
        mat->SetProperty("intvar1", 0);
        mat->CheckUpdatePropertyBuffers();
    }

    void GUIBackend::DrawSimpleText(LinaVG::SimpleTextDrawBuffer* buf, int thread)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::SimpleText, thread);
        mat->SetProperty("intvar1", 3);
        mat->SetTexture("diffuse", m_fontTextures[buf->m_textureHandle]);
        mat->CheckUpdatePropertyBuffers();
    }

    void GUIBackend::DrawSDFText(LinaVG::SDFTextDrawBuffer* buf, int thread)
    {
        Material*   mat              = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::SDF, thread);
        const float thickness        = 1.0f - Math::Clamp(buf->m_thickness, 0.0f, 1.0f);
        const float softness         = Math::Clamp(buf->m_softness, 0.0f, 10.0f) * 0.1f;
        const float outlineThickness = Math::Clamp(buf->m_outlineThickness, 0.0f, 1.0f);
        mat->SetProperty("intvar1", 4);
        mat->SetProperty("color1", TO_LINA_VEC4(buf->m_outlineColor));
        mat->SetProperty("floatvar3", outlineThickness);
        mat->SetProperty("floatvar2", softness);
        mat->SetProperty("floatvar1", thickness);
        mat->SetProperty("intvar3", buf->m_flipAlpha);
        mat->SetProperty("intvar2", buf->m_outlineThickness != 0.0f ? 1 : 0);
        mat->SetTexture("diffuse", m_fontTextures[buf->m_textureHandle]);
        mat->CheckUpdatePropertyBuffers();
    }

    Material* GUIBackend::AddOrderedDrawRequest(LinaVG::DrawBuffer* buf, LinaVGDrawCategoryType type, int thread)
    {
        auto& capsule = m_bufferCapsules[thread];

        const Vector2i     swpSize = capsule.swapchain->size;
        OrderedDrawRequest request;
        request.firstIndex   = capsule.indexCounter;
        request.vertexOffset = capsule.vertexCounter;
        request.type         = type;
        request.indexSize    = static_cast<uint32>(buf->m_indexBuffer.m_size);
        request.meta.clipX   = buf->clipPosX;
        request.meta.clipY   = buf->clipPosY;
        request.meta.clipW   = buf->clipSizeX == 0 ? swpSize.x : buf->clipSizeX;
        request.meta.clipH   = buf->clipSizeY == 0 ? swpSize.y : buf->clipSizeY;

        auto& pool = capsule.pools[m_frameIndex];

        uint32 matId = pool.index;

        if (matId >= pool.materials.size())
        {
            LINA_ASSERT(false, "[GUI Backend] -> Insufficient material pool size!");
        }
        else
            pool.index++;

        auto mat = pool.materials[matId];
        mat->SetProperty("projection", capsule.projection);
        request.matId = matId;

        capsule.orderedDrawRequests.push_back(request);
        capsule.vtxBuffer.CopyIntoPadded(buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex), capsule.vertexCounter * sizeof(LinaVG::Vertex));
        capsule.indxBuffer.CopyIntoPadded(buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index), capsule.indexCounter * sizeof(LinaVG::Index));
        capsule.indexCounter += static_cast<uint32>(buf->m_indexBuffer.m_size);
        capsule.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
        return mat;
    }

    void GUIBackend::EndFrame()
    {
        for (auto& capsule : m_bufferCapsules)
            capsule.pools[m_frameIndex].index = 0;
    }

    void GUIBackend::RecordDrawCommands(int thread)
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        LinaVG::Render(thread);

        auto& capsule = m_bufferCapsules[thread];

        if (capsule.orderedDrawRequests.empty())
            return;

        uint64 offset = 0;
        capsule.cmd->CMD_BindVertexBuffers(0, 1, capsule.vtxBuffer._ptr, &offset);
        capsule.cmd->CMD_BindIndexBuffers(capsule.indxBuffer._ptr, 0, IndexType::Uint32);
        m_guiStandard->Bind(*capsule.cmd, MaterialBindFlag::BindPipeline);

        for (auto& r : capsule.orderedDrawRequests)
        {
            Recti rect;
            rect.pos.x  = r.meta.clipX;
            rect.pos.y  = r.meta.clipY;
            rect.size.x = r.meta.clipW;
            rect.size.y = r.meta.clipH;
            capsule.cmd->CMD_SetScissors(rect);
            capsule.pools[m_frameIndex].materials[r.matId]->Bind(*capsule.cmd, MaterialBindFlag::BindDescriptor);
            capsule.cmd->CMD_DrawIndexed(r.indexSize, 1, r.firstIndex, r.vertexOffset, 0);
        }

        capsule.orderedDrawRequests.clear();

        // End frame, important.
        capsule.indexCounter = capsule.vertexCounter = 0;
    }

    LinaVG::BackendHandle GUIBackend::CreateFontTexture(int width, int height)
    {
        Texture* txt     = new Texture();
        Sampler  sampler = Sampler{
             .minFilter     = Filter::Linear,
             .magFilter     = Filter::Linear,
             .u             = SamplerAddressMode::ClampToEdge,
             .v             = SamplerAddressMode::ClampToEdge,
             .w             = SamplerAddressMode::ClampToEdge,
             .mipLodBias    = 0.0f,
             .minLod        = 0.0f,
             .maxLod        = 1.0f,
             .maxAnisotropy = 1.0f,
             .borderColor   = BorderColor::FloatOpaqueWhite,
        };
        txt->GenerateCustomBuffers(width, height, 1, 1, Format::R8_UNORM, sampler, ImageTiling::Linear);
        txt->SetUserManaged(true);

        // Skipping 0 index
        const uint32 handle    = static_cast<uint32>(m_fontTextures.size() + 1);
        m_fontTextures[handle] = txt;
        m_bufferingFontTexture = handle;

        String fontTextureID = "Font Texture" + TO_STRING(handle);
        txt->m_sid           = TO_SID(fontTextureID);
        Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(txt->m_sid, txt);
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

    void GUIBackend::UploadAllFontTextures()
    {
        for (auto& t : m_fontTextures)
        {
            Texture* txt = t.second;
            txt->WriteToGPUImage(m_renderEngine->GetGPUUploader(), Offset3D{.x = 0, .y = 0, .z = 0}, txt->m_extent, true);
        }
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

    void GUIBackend::Prepare(uint32 frameIndex)
    {
        m_frameIndex = frameIndex;
    }

    void GUIBackend::SetSwapchain(int thread, Swapchain* swp)
    {
       
    }

    void GUIBackend::SetCmd(int thread, Swapchain* swapchain, CommandBuffer* cmd)
    {
        m_bufferCapsules[thread].cmd       = cmd;
        m_bufferCapsules[thread].swapchain = swapchain;
        UpdateProjection(thread, swapchain->size);
    }

    void GUIBackend::UpdateProjection(int thread, const Vector2i& size)
    {
        auto& capsule = m_bufferCapsules[thread];
        if (size.Equals(capsule.lastProjectionSize))
            return;

        capsule.lastProjectionSize = size;
        const Vector2i pos         = Vector2i();

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

        capsule.projection[0][0] = 2.0f / (R - L);
        capsule.projection[0][1] = 0.0f;
        capsule.projection[0][2] = 0.0f;
        capsule.projection[0][3] = 0.0f;
        capsule.projection[1][0] = 0.0f;
        capsule.projection[1][1] = 2.0f / (T - B);
        capsule.projection[1][2] = 0.0f;
        capsule.projection[1][3] = 0.0f;
        capsule.projection[2][0] = 0.0f;
        capsule.projection[2][1] = 0.0f;
        capsule.projection[2][2] = -1.0f;
        capsule.projection[2][3] = 0.0f;
        capsule.projection[3][0] = (R + L) / (L - R);
        capsule.projection[3][1] = (T + B) / (B - T);
        capsule.projection[3][2] = 0.0f;
        capsule.projection[3][3] = 1.0f;
    }

} // namespace Lina::Graphics
