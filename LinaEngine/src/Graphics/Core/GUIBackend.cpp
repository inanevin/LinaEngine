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
#include "Graphics/Platform/LinaVGIncl.hpp"
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

        for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
            CreateBufferCapsule(0, false);

        return true;
    }

    void GUIBackend::CreateBufferCapsule(StringID sid, bool setMaterials)
    {
        m_bufferCapsules[sid].push_back(BufferCapsule());
        auto& caps = m_bufferCapsules[sid][m_bufferCapsules[sid].size() - 1];

        caps.vtxBuffer = Buffer{
            .size          = BUFFER_LIMIT,
            .bufferUsage   = GetBufferUsageFlags(BufferUsageFlags::VertexBuffer),
            .memoryUsage   = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::HostVisible,
        };

        caps.indxBuffer = Buffer{
            .size          = BUFFER_LIMIT,
            .bufferUsage   = GetBufferUsageFlags(BufferUsageFlags::IndexBuffer),
            .memoryUsage   = MemoryUsageFlags::CpuToGpu,
            .requiredFlags = MemoryPropertyFlags::HostVisible,
        };

        caps.indxBuffer.Create();
        caps.vtxBuffer.Create();

        if (setMaterials)
        {
            auto& pool = caps.materialPool;
            pool.materials.resize(MATERIAL_POOL_COUNT, nullptr);
            pool.index = 0;

            for (auto& m : pool.materials)
            {
                m = new Material();
                m->CreateBuffer();
                m->SetShader(m_guiStandard->GetShaderHandle().value);
            }
        }
    }

    void GUIBackend::RemoveBufferCapsule(StringID sid)
    {
        auto it = m_bufferCapsules.find(sid);

        for (auto c : it->second)
        {
            c.indxBuffer.Destroy();
            c.vtxBuffer.Destroy();

            for (auto m : c.materialPool.materials)
                delete m;
        }

        m_bufferCapsules.erase(it);
    }

    void GUIBackend::OnPreMainLoop(const Event::EPreMainLoop& ev)
    {
        if (m_guiStandard == nullptr)
            m_guiStandard = Lina::Graphics::RenderEngine::Get()->GetEngineMaterial(Lina::Graphics::EngineShaderType::GUIStandard);

        for (auto& pair : m_bufferCapsules)
        {
            for (auto& b : pair.second)
            {
                auto& pool = b.materialPool;
                pool.materials.resize(MATERIAL_POOL_COUNT, nullptr);
                pool.index = 0;

                for (auto& m : pool.materials)
                {
                    m = new Material();
                    m->CreateBuffer();
                    m->SetShader(m_guiStandard->GetShaderHandle().value);
                }
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
            for (auto& c : b.second)
            {
                c.indxBuffer.Destroy();
                c.vtxBuffer.Destroy();
            }
        }
    }

    void GUIBackend::StartFrame(int threadCount)
    {
    }

    void GUIBackend::DrawGradient(LinaVG::GradientDrawBuffer* buf)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Gradient);
        mat->SetProperty("intvar1", 1);
        mat->SetProperty("color1", TO_LINA_VEC4(buf->m_color.start));
        mat->SetProperty("color2", TO_LINA_VEC4(buf->m_color.end));
        mat->SetProperty("intvar2", static_cast<int>(buf->m_color.gradientType));
        mat->SetProperty("floatvar1", buf->m_color.radialSize);
        mat->SetProperty("intvar3", buf->m_isAABuffer);
        mat->CheckUpdatePropertyBuffers();
    }

    void GUIBackend::DrawTextured(LinaVG::TextureDrawBuffer* buf)
    {
        Material* mat = AddOrderedDrawRequest(buf, LinaVGDrawCategoryType::Textured);
        Texture*  txt = Resources::ResourceManager::Get()->GetResource<Texture>(buf->m_textureHandle);

        mat->SetProperty("intvar1", 2);
        mat->SetProperty("vec2pack1", Vector2(buf->m_textureUVTiling.x, buf->m_textureUVTiling.y));
        mat->SetProperty("vec2pack2", Vector2(buf->m_textureUVOffset.x, buf->m_textureUVOffset.y));
        mat->SetProperty("color1", Vector4(buf->m_tint.x, buf->m_tint.y, buf->m_tint.z, buf->m_tint.w));
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
        mat->SetProperty("color1", TO_LINA_VEC4(buf->m_outlineColor));
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
        auto& targetCapsule = GetCurrentBufferCapsule();

        const Vector2i     swpSize = m_currentSwapchainIndexPair.first->size;
        OrderedDrawRequest request;
        request.firstIndex   = targetCapsule.indexCounter;
        request.vertexOffset = targetCapsule.vertexCounter;
        request.type         = type;
        request.indexSize    = static_cast<uint32>(buf->m_indexBuffer.m_size);
        request.meta.clipX   = buf->clipPosX;
        request.meta.clipY   = buf->clipPosY;
        request.meta.clipW   = buf->clipSizeX == 0 ? swpSize.x : buf->clipSizeX;
        request.meta.clipH   = buf->clipSizeY == 0 ? swpSize.y : buf->clipSizeY;

        auto& pool = targetCapsule.materialPool;

        uint32 matId = pool.index;

        LINA_TRACE("Adding Draw request {0}", matId);
        
        if (matId >= pool.materials.size())
        {
            LINA_ASSERT(false, "[GUI Backend] -> Insufficient material pool size!");
        }
        else
            pool.index++;

        auto mat = pool.materials[matId];
        mat->SetProperty("projection", m_projection);
        request.matId = matId;

        targetCapsule.orderedDrawRequests.push_back(request);
        targetCapsule.vtxBuffer.CopyIntoPadded(buf->m_vertexBuffer.m_data, buf->m_vertexBuffer.m_size * sizeof(LinaVG::Vertex), targetCapsule.vertexCounter * sizeof(LinaVG::Vertex));
        targetCapsule.indxBuffer.CopyIntoPadded(buf->m_indexBuffer.m_data, buf->m_indexBuffer.m_size * sizeof(LinaVG::Index), targetCapsule.indexCounter * sizeof(LinaVG::Index));

        targetCapsule.indexCounter += static_cast<uint32>(buf->m_indexBuffer.m_size);
        targetCapsule.vertexCounter += static_cast<uint32>(buf->m_vertexBuffer.m_size);
        return mat;
    }

    void GUIBackend::Reset()
    {
        for (auto& b : m_bufferCapsules)
        {
            for (auto& c : b.second)
                c.materialPool.index = 0;
        }
    }

    void GUIBackend::EndFrame()
    {
    }

    void GUIBackend::RecordDrawCommands()
    {
        PROFILER_FUNC(PROFILER_THREAD_RENDER);

        LINA_TRACE("STARTING LINA VG RENDER");
        LinaVG::Render();
        LINA_TRACE("ENDING LINA VG RENDER");

        auto& b = GetCurrentBufferCapsule();

        if (b.orderedDrawRequests.empty())
            return;

        uint64 offset = 0;
        m_cmd->CMD_BindVertexBuffers(0, 1, b.vtxBuffer._ptr, &offset);
        m_cmd->CMD_BindIndexBuffers(b.indxBuffer._ptr, 0, IndexType::Uint32);
        m_guiStandard->Bind(*m_cmd, MaterialBindFlag::BindPipeline);

        for (auto& r : b.orderedDrawRequests)
        {
            Recti rect;
            rect.pos.x  = r.meta.clipX;
            rect.pos.y  = r.meta.clipY;
            rect.size.x = r.meta.clipW;
            rect.size.y = r.meta.clipH;
            m_cmd->CMD_SetScissors(rect);
            b.materialPool.materials[r.matId]->Bind(*m_cmd, MaterialBindFlag::BindDescriptor);
            m_cmd->CMD_DrawIndexed(r.indexSize, 1, r.firstIndex, r.vertexOffset, 0);
        }

        b.orderedDrawRequests.clear();

        // End frame, important.
        b.indexCounter = b.vertexCounter = 0;
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
            txt->WriteToGPUImage(Offset3D{.x = 0, .y = 0, .z = 0}, txt->m_extent, true);
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

    void GUIBackend::Prepare(Swapchain* swapchain, uint32 frameIndex, CommandBuffer* cmd)
    {
        m_currentSwapchainIndexPair.first  = swapchain;
        m_currentSwapchainIndexPair.second = frameIndex;
        m_cmd                              = cmd;
        UpdateProjection(swapchain->size);
    }

    void GUIBackend::UpdateProjection(const Vector2i& size)
    {
        if (size.Equals(m_lastProjectionSize))
            return;

        m_lastProjectionSize = size;
        const Vector2i pos   = Vector2i();

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

    GUIBackend::BufferCapsule& GUIBackend::GetCurrentBufferCapsule()
    {
        const StringID id = m_currentSwapchainIndexPair.first->swapchainID;
        if (m_bufferCapsules.find(id) == m_bufferCapsules.end())
        {
            for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
                CreateBufferCapsule(id, true);
        }

        return m_bufferCapsules[id][m_currentSwapchainIndexPair.second];
    }

} // namespace Lina::Graphics
