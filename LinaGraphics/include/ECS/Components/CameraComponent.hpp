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
