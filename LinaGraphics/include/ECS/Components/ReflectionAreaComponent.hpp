/*
Class: ReflectionArea



Timestamp: 1/23/2022 1:54:46 AM
*/

#pragma once

#ifndef ReflectionArea_HPP
#define ReflectionArea_HPP

// Headers here.
#include "ECS/Component.hpp"
#include "Math/Vector.hpp"
#include "Rendering/Texture.hpp"

namespace Lina::ECS
{
    LINA_COMPONENT("Reflection Area", "ICON_FA_MOUNTAIN", "Rendering", "true")
    struct ReflectionAreaComponent : public Component
    {
        LINA_PROPERTY("Is Local", "Bool", "If false, any object not affected by a local area will be affected by this one.")
        bool m_isLocal = false;

        LINA_PROPERTY("Is Dynamic", "Bool", "If true, the reflection data will be captured every frame for this area.")
        bool m_isDynamic = true;

        LINA_PROPERTY("Half Extents", "Vector3", "", "m_isLocal")
        Vector3 m_halfExtents = Vector3(.5f, .5f, .5f);

        LINA_PROPERTY("Resolution", "Vector2i")
        Vector2i m_resolution = Vector2i(512,512);

        LINA_PROPERTY("Draw Debug", "Bool", "Enables debug drawing for this component.")
        bool m_drawDebug = true;

        Graphics::Texture m_cubemap;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_isLocal, m_halfExtents, m_isDynamic, m_resolution, m_isEnabled); // serialize things by passing them to the archive
        }
    };
} // namespace Lina::ECS

#endif
