/*
Class: LightComponent

All light types are declared here.

Timestamp: 5/13/2019 9:00:55 PM
*/

#pragma once

#ifndef LightComponent_HPP
#define LightComponent_HPP

#include "ECS/Component.hpp"
#include "Math/Color.hpp"
#include "Math/Math.hpp"
#include "Math/Vector.hpp"

namespace Lina::ECS
{
    LINA_COMPONENT("Light Component", "ICON_FA_EYE", "Lights", "true")
    struct LightComponent : public Component
    {
        LINA_PROPERTY("Color", "Color")
        Color m_color = Color::White;

        LINA_PROPERTY("Intensity", "Float", "", "")
        float m_intensity = 1.0f;

        LINA_PROPERTY("Draw Debug", "Bool", "Enables debug drawing for this component.")
        bool m_drawDebug = true;

        LINA_PROPERTY("Cast Shadows", "Bool", "Enables dynamic shadow casting for this light.")
        bool m_castsShadows = false;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_color, m_isEnabled); // serialize things by passing them to the archive
        }
    };

    LINA_COMPONENT("Point Light Component", "ICON_FA_EYE", "Lights", "true", "LightComponent")
    struct PointLightComponent : public LightComponent
    {
        LINA_PROPERTY("Distance", "Float", "Light Distance")
        float m_distance = 25.0f;

        LINA_PROPERTY("Bias", "Float", "Defines the shadow crispiness.", "m_castsShadows")
        float m_bias = 0.3f;

        LINA_PROPERTY("Shadow Near", "Float", "", "m_castsShadows")
        float m_shadowNear = 0.1f;

        LINA_PROPERTY("Shadow Far", "Float", "", "m_castsShadows")
        float m_shadowFar = 25.0f;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_distance, m_intensity, m_shadowNear, m_shadowFar, m_bias, m_color, m_drawDebug, m_isEnabled, m_castsShadows); // serialize things by passing them to the archive
        }
    };

    LINA_COMPONENT("Spot Light Component", "ICON_FA_EYE", "Lights", "true", "LightComponent")
    struct SpotLightComponent : public LightComponent
    {
        LINA_PROPERTY("Distance", "Float", "Light Distance")
        float m_distance = 0;

        LINA_PROPERTY("Cutoff", "Float", "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff.")
        float m_cutoff = Math::Cos(Math::ToRadians(12.5f));

        LINA_PROPERTY("Outer Cutoff", "Float", "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff.")
        float m_outerCutoff = Math::Cos(Math::ToRadians(17.5f));

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_color, m_castsShadows, m_intensity, m_drawDebug, m_distance, m_cutoff, m_outerCutoff, m_isEnabled); // serialize things by passing them to the archive
        }
    };

    LINA_COMPONENT("Directional Light Component", "ICON_FA_EYE", "Lights", "true", "LightComponent")
    struct DirectionalLightComponent : public LightComponent
    {
        LINA_PROPERTY("Projection", "Vector4", "Defines shadow projection boundaries.", "m_castsShadows")
        Vector4 m_shadowOrthoProjection = Vector4(-20, 20, -20, 20);

        LINA_PROPERTY("Shadow Near", "Float", "", "m_castsShadows")
        float m_shadowZNear = 10.0f;

        LINA_PROPERTY("Shadow Far", "Float", "", "m_castsShadows")
        float m_shadowZFar = 15.0f;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_shadowOrthoProjection, m_castsShadows, m_intensity, m_drawDebug, m_shadowZNear, m_shadowZFar, m_color, m_isEnabled); // serialize things by passing them to the archive
        }
    };
} // namespace Lina::ECS

#endif
