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

#include "ECS/Systems/ReflectionSystem.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ReflectionAreaComponent.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderConstants.hpp"
#include "Core/RenderEngineBackend.hpp"

namespace Lina::ECS
{
    void ReflectionSystem::Initialize(const std::string& name, ApplicationMode& appMode)
    {
        System::Initialize(name);
        Event::EventSystem::Get()->Connect<Event::EPlayModeChanged, &ReflectionSystem::OnPlayModeChanged>(this);
        m_renderEngine = Graphics::RenderEngineBackend::Get();
        m_appMode      = appMode;
    }

    void ReflectionSystem::UpdateReflectionData()
    {
        auto* reg  = Registry::Get();
        auto& view = reg->view<EntityDataComponent, ReflectionAreaComponent>();
        m_poolSize = (int)view.size_hint();

        for (auto entity : view)
        {
            auto& refArea = view.get<ReflectionAreaComponent>(entity);
            auto& data    = view.get<EntityDataComponent>(entity);

            // Make sure the area's texture is constructed.
            if (refArea.m_cubemap.GetIsEmpty())
                ConstructRefAreaTexture(&refArea.m_cubemap, refArea.m_resolution);

            if (refArea.m_isDynamic)
            {
                m_renderEngine->CaptureReflections(refArea.m_cubemap, data.GetLocation(), refArea.m_resolution);
            }
        }
    }

    void ReflectionSystem::SetReflectionsOnMaterial(Graphics::Material* mat, const Vector3& transformLocation)
    {
        auto* reg = Registry::Get();

        // Check if the given location is inside any of the reflection area's extents.

        auto& view = reg->view<EntityDataComponent, ReflectionAreaComponent>();

        for (auto entity : view)
        {
            auto&         refArea = view.get<ReflectionAreaComponent>(entity);
            auto&         data    = view.get<EntityDataComponent>(entity);
            const Vector3 max     = data.GetLocation() + refArea.m_halfExtents;
            const Vector3 min     = data.GetLocation() - refArea.m_halfExtents;
            if (!refArea.m_isLocal || (transformLocation.x > min.x && transformLocation.x < max.x && transformLocation.y > min.y && transformLocation.y < max.y && transformLocation.z > min.z && transformLocation.z < max.z))
            {
                if (mat->m_sampler2Ds.find(MAT_MAP_REFLECTIONAREAMAP) != mat->m_sampler2Ds.end())
                {
                    mat->SetTexture(MAT_MAP_REFLECTIONAREAMAP, &refArea.m_cubemap, Graphics::TextureBindMode::BINDTEXTURE_CUBEMAP);
                    mat->m_reflectionDataSet = true;
                }
            }
            else
            {
                if (mat->m_reflectionDataSet)
                {
                    // remove data.
                    mat->RemoveTexture(MAT_MAP_REFLECTIONAREAMAP);
                }
            }
        }
    }

    void ReflectionSystem::ConstructRefAreaTexture(Graphics::Texture* texture, const Vector2i& res)
    {
        Graphics::SamplerParameters samplerParams;
        samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = Graphics::SamplerWrapMode::WRAP_CLAMP_EDGE;
        samplerParams.m_textureParams.m_magFilter                                                                             = Graphics::SamplerFilter::FILTER_LINEAR;
        samplerParams.m_textureParams.m_minFilter                                                                             = Graphics::SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
        samplerParams.m_textureParams.m_internalPixelFormat                                                                   = Graphics::PixelFormat::FORMAT_RGB16F;
        samplerParams.m_textureParams.m_pixelFormat                                                                           = Graphics::PixelFormat::FORMAT_RGB;
        texture->ConstructRTCubemapTexture(res, samplerParams);
    }

    void ReflectionSystem::OnPlayModeChanged(const Event::EPlayModeChanged& ev)
    {
        auto* reg = Registry::Get();

        // Find all the reflection area's that are supposed to be calculated when the game begins.
        if (ev.m_playMode)
        {
            auto& view = reg->view<EntityDataComponent, ReflectionAreaComponent>();

            for (auto entity : view)
            {
                auto& refArea = view.get<ReflectionAreaComponent>(entity);
                auto& data    = view.get<EntityDataComponent>(entity);

                if (!refArea.m_isDynamic)
                {
                    // Tell render engine to render the scene by writing to the reflection area's cubemap texture.
                    m_renderEngine->CaptureReflections(refArea.m_cubemap, data.GetLocation(), refArea.m_resolution);
                }
            }
        }
    }
} // namespace Lina::ECS
