/*
Class: RenderSettings

Contains the internal render settings, responsible for their serialization.

Timestamp: 10/30/2020 9:17:05 AM
*/

#pragma once

#ifndef RenderSettings_HPP
#define RenderSettings_HPP

#include <Data/String.hpp>
#include "Core/CommonReflection.hpp"

namespace Lina::Graphics
{
    LINA_CLASS("Render Settings")
    class RenderSettings
    {

    public:
        RenderSettings()  = default;
        ~RenderSettings() = default;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_bloomEnabled, m_fxaaEnabled, m_fxaaReduceMin, m_fxaaReduceMul, m_fxaaSpanMax, m_gamma, m_exposure, m_vignetteEnabled, m_vignetteAmount, m_vignettePow);
        }

        LINA_PROPERTY("Gamma", "Float", "", "", "Tonemapping")
        float m_gamma = 2.2f;

        LINA_PROPERTY("Exposure", "Float", "", "", "Tonemapping")
        float m_exposure = 1.0f;

        LINA_PROPERTY("Bloom", "Bool", "", "", "Bloom")
        bool m_bloomEnabled = false;

        LINA_PROPERTY("FXAA", "Bool", "Enable fast approximate anti-aliasing.", "", "Anti-Aliasing")
        bool m_fxaaEnabled = false;

        LINA_PROPERTY("Reduce Min", "Float", "", "m_fxaaEnabled", "Anti-Aliasing")
        float m_fxaaReduceMin = 1.0f / 128.0f;

        LINA_PROPERTY("Reduce Mul", "Float", "", "m_fxaaEnabled", "Anti-Aliasing")
        float m_fxaaReduceMul = 1.0f / 8.0f;

        LINA_PROPERTY("Reduce Span", "Float", "", "m_fxaaEnabled", "Anti-Aliasing")
        float m_fxaaSpanMax = 8.0f;

        LINA_PROPERTY("Vignette", "Bool", "", "", "Vignette")
        bool m_vignetteEnabled = false;

        LINA_PROPERTY("Amount", "Float", "", "m_vignetteEnabled", "Vignette")
        float m_vignetteAmount = 25.0f;

        LINA_PROPERTY("Pow", "Float", "", "m_vignetteEnabled", "Vignette")
        float m_vignettePow = 0.75f;
    };
} // namespace Lina::Graphics

#endif
