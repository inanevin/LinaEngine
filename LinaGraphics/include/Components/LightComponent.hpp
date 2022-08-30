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

#pragma once

#ifndef LightComponent_HPP
#define LightComponent_HPP

#include "Core/Component.hpp"
#include "Core/CommonReflection.hpp"
#include "Math/Color.hpp"
#include "Math/Math.hpp"
#include "Math/Vector.hpp"

namespace Lina::Graphics
{
    LINA_COMPONENT("Light Component", "Lights", "True")
    class LightComponent : public World::Component
    {
    public:
        LINA_FIELD("Color", "Color")
        Color color = Color::White;

        LINA_FIELD("Intensity", "Float", "", "")
        float intensity = 1.0f;

        virtual void SaveToArchive(Serialization::Archive<OStream>& oarchive) override
        {
            World::Component::SaveToArchive(oarchive);
            oarchive(color, intensity);
        }

        virtual void LoadFromArchive(Serialization::Archive<IStream>& iarchive) override
        {
            World::Component::LoadFromArchive(iarchive);
            iarchive(color, intensity);
        }
    };

    LINA_COMPONENT("Point Light Component", "Lights")
    class PointLightComponent : public LightComponent
    {
    public:
        LINA_FIELD("Distance", "Float", "Light Distance")
        float distance = 25.0f;

        virtual TypeID GetTID() override
        {
            return GetTypeID<PointLightComponent>();
        }

        virtual void SaveToArchive(Serialization::Archive<OStream>& oarchive) override
        {
            LightComponent::SaveToArchive(oarchive);
            oarchive(distance);
        }

        virtual void LoadFromArchive(Serialization::Archive<IStream>& iarchive) override
        {
            LightComponent::LoadFromArchive(iarchive);
            iarchive(distance);
        }
    };

    LINA_COMPONENT("Spot Light Component", "Lights")
    class SpotLightComponent : public LightComponent
    {
    public:
        LINA_FIELD("Distance", "Float", "Light Distance")
        float distance = 0;

        LINA_FIELD("Cutoff", "Float", "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff.")
        float cutoff = Math::Cos(Math::ToRadians(12.5f));

        LINA_FIELD("Outer Cutoff", "Float", "The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff.")
        float outerCutoff = Math::Cos(Math::ToRadians(17.5f));

        virtual TypeID GetTID() override
        {
            return GetTypeID<SpotLightComponent>();
        }

        virtual void SaveToArchive(Serialization::Archive<OStream>& oarchive) override
        {
            LightComponent::SaveToArchive(oarchive);
            oarchive(distance, cutoff, outerCutoff);
        }

        virtual void LoadFromArchive(Serialization::Archive<IStream>& iarchive) override
        {
            LightComponent::LoadFromArchive(iarchive);
            iarchive(distance, cutoff, outerCutoff);
        }
    };

    LINA_COMPONENT("Directional Light Component", "Lights")
    class DirectionalLightComponent : public LightComponent
    {
    public:
        virtual TypeID GetTID() override
        {
            return GetTypeID<DirectionalLightComponent>();
        }

        virtual void SaveToArchive(Serialization::Archive<OStream>& oarchive) override
        {
            LightComponent::SaveToArchive(oarchive);
        }

        virtual void LoadFromArchive(Serialization::Archive<IStream>& iarchive) override
        {
            LightComponent::LoadFromArchive(iarchive);
        }
    };
} // namespace Lina::ECS

#endif