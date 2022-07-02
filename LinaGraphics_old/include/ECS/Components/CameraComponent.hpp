/*
Class: CameraComponent

Represents a virtual game camera.

Timestamp: 5/2/2019 12:20:42 AM
*/

#pragma once

#ifndef CameraComponent_HPP
#define CameraComponent_HPP

#include "ECS/Component.hpp"
#include "Math/Color.hpp"
#include "Math/Frustum.hpp"

namespace Lina::ECS
{
    LINA_COMPONENT("Camera Component", "ICON_FA_EYE", "Rendering", "true", "true")
    struct CameraComponent : public Component
    {
        LINA_PROPERTY("Clear Color", "Color")
        Color m_clearColor = Color(0.1f, 0.1f, 0.1f, 1.0f);

        LINA_PROPERTY("Field of View", "Float")
        float m_fieldOfView = 90.0f;

        LINA_PROPERTY("Near", "Float", "Minimum distance the camera renders at.")
        float m_zNear = 0.01f;

        LINA_PROPERTY("Far", "Float", "Maximum distance the camera renders at.")
        float m_zFar = 1000.0f;
        
        LINA_PROPERTY("Is Active", "Bool")
        bool m_isActive = false;

        Frustum m_viewFrustum;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_clearColor, m_fieldOfView, m_zNear, m_zFar, m_isActive, m_isEnabled); // serialize things by passing them to the archive
        }
    };
} // namespace Lina::ECS

#endif
