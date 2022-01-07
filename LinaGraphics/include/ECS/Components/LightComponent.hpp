/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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
    LINA_COMPONENT("Light Component", "ICON_FA_EYE", "Lights", "true", "true", "")
    struct LightComponent : public Component
    {
        LINA_PROPERTY("Color", "Color", "", "")
        Color m_color = Color::White;

        LINA_PROPERTY("Intensity", "Float", "", "")
        float m_intensity = 1.0f;

        LINA_PROPERTY("Draw Debug", "Bool", "Enables debug drawing for this component.", "")
        bool m_drawDebug = true;

        LINA_PROPERTY("Cast Shadows", "Bool", "Enables dynamic shadow casting for this light.", "")
        bool m_castsShadows = false;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_color, m_isEnabled); // serialize things by passing them to the archive
        }
    };

    LINA_COMPONENT("Point Light Component", "ICON_FA_EYE", "Lights", "true", "true", "LightComponent")
    struct PointLightComponent : public LightComponent
    {
        LINA_PROPERTY("Distance", "Float", "Light Distance", "")
        float m_distance = 25.0f;

        LINA_PROPERTY("Bias", "Float", "Defines the shadow crispiness.", "")
        float m_bias = 0.3f;

        LINA_PROPERTY("Shadow Near", "Float", "", "")
        float m_shadowNear = 0.1f;

        LINA_PROPERTY("Shadow Far", "Float", "", "")
        float m_shadowFar = 25.0f;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_distance, m_intensity, m_shadowNear, m_shadowFar, m_bias, m_color, m_drawDebug, m_isEnabled, m_castsShadows); // serialize things by passing them to the archive
        }
    };

    LINA_COMPONENT("Spot Light Component", "ICON_FA_EYE", "Lights", "true", "true", "LightComponent")
    struct SpotLightComponent : public LightComponent
    {
        LINA_PROPERTY("Distance", "Float", "Light Distance", "")
        float m_distance = 0;

        LINA_PROPERTY("Cutoff", "Float", "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff.", "")
        float m_cutoff = Math::Cos(Math::ToRadians(12.5f));

        LINA_PROPERTY("Outer Cutoff", "Float", "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff.", "")
        float m_outerCutoff = Math::Cos(Math::ToRadians(17.5f));

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_color, m_castsShadows, m_intensity, m_drawDebug, m_distance, m_cutoff, m_outerCutoff, m_isEnabled); // serialize things by passing them to the archive
        }
    };

    LINA_COMPONENT("Directional Light Component", "ICON_FA_EYE", "Lights", "true", "true", "LightComponent")
    struct DirectionalLightComponent : public LightComponent
    {
        LINA_PROPERTY("Projection", "Vector4", "Defines shadow projection boundaries.", "")
        Vector4 m_shadowOrthoProjection = Vector4(-20, 20, -20, 20);

        LINA_PROPERTY("Shadow Near", "Float", "", "")
        float m_shadowZNear = 10.0f;

        LINA_PROPERTY("Shadow Far", "Float", "", "")
        float m_shadowZFar = 15.0f;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_shadowOrthoProjection, m_castsShadows, m_intensity, m_drawDebug, m_shadowZNear, m_shadowZFar, m_color, m_isEnabled); // serialize things by passing them to the archive
        }
    };
} // namespace Lina::ECS

#endif
