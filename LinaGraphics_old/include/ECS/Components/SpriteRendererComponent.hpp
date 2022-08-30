/*
Class: SpriteRendererComponent
Timestamp: 10/1/2020 9:26:21 AM
*/

#pragma once

#ifndef SpriteRendererComponent_HPP
#define SpriteRendererComponent_HPP

#include "ECS/Component.hpp"

#include <Data/String.hpp>

namespace Lina::ECS
{
    LINA_COMPONENT("Sprite Component", "ICON_FA_EYE", "Rendering", "true", "true")
    struct SpriteRendererComponent : public Component
    {
        int    m_materialID    = -1;
        String m_materialPaths = "";

        // Editor properties, not inside the macro to avoid any struct size mismatch during serialization.
        int    m_selectedMatID   = -1;
        String m_selectedMatPath = "";

        template <class Archive>
        void Serialize(Archive& archive)
        {
            archive(m_materialPaths, m_isEnabled); // serialize things by passing them to the archive
        }
    };
} // namespace Lina::ECS

#endif
