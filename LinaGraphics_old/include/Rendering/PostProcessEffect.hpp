/*
Class: PostProcessEffect



Timestamp: 12/10/2020 6:02:07 PM
*/

#pragma once

#ifndef PostProcessEffect_HPP
#define PostProcessEffect_HPP

// Headers here.
#include "Core/RenderEngineFwd.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderTarget.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"

namespace Lina::Graphics
{

    class PostProcessEffect
    {

    public:
        PostProcessEffect() = default;
        ~PostProcessEffect() = default;

        void      Construct(Shader* shader);
        void      Draw(Texture* screenMap);
        Material& GetMaterial()
        {
            return m_material;
        }
        Texture& GetTexture()
        {
            return m_rtTexture;
        }
        RenderTarget& GetRenderTarget()
        {
            return m_renderTarget;
        }
        SamplerParameters& GetParams()
        {
            return m_samplerParams;
        }

    private:
        RenderDevice* m_renderDevice = nullptr;
        RenderEngine* m_renderEngine = nullptr;

        DrawParams        m_drawParams;
        Texture           m_rtTexture;
        RenderTarget      m_renderTarget;
        SamplerParameters m_samplerParams;
        Material          m_material;
    };
} // namespace Lina::Graphics

#endif
