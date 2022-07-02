/*
Class: Sampler

Represents a data structure for texture samplers created in the GPU backend.

Timestamp: 4/26/2019 6:20:27 PM
*/

#pragma once

#ifndef Sampler_HPP
#define Sampler_HPP

#include "Core/RenderEngineFwd.hpp"
#include "Rendering/RenderingCommon.hpp"

namespace Lina::Graphics
{
    class Sampler
    {
    public:
        Sampler() = default;

        // Destructor releases sampler data through render engine
        ~Sampler();

        void Construct(SamplerParameters samplerParams, TextureBindMode bindMode, bool isCubemap = false);
        void UpdateSettings(SamplerParameters samplerParams);

        SamplerParameters& GetSamplerParameters()
        {
            return m_params;
        }
        uint32 GetID() const
        {
            return m_engineBoundID;
        }
        void SetTargetTextureID(uint32 id)
        {
            m_targetTextureID = id;
        }

    private:
        uint32            m_targetTextureID = 0;
        TextureBindMode   m_targetBindMode;
        SamplerParameters m_params;
        RenderDevice*     m_renderDevice  = nullptr;
        uint32            m_engineBoundID = 0;
    };
} // namespace Lina::Graphics

#endif
