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

#include "ECS/Components/EntityDataComponent.hpp"
#include "Log/Log.hpp"
#include "ECS/Registry.hpp"

namespace Lina::ECS
{
    void EntityDataComponent::SetIsEnabled(bool isEnabled)
    {
        LINA_WARN("Setting the enabled state of data component does nothing. If you are trying to disable/enable the entity, use the Registry::SetEntityEnabled()");
    }

    void EntityDataComponent::SetTransformation(Matrix& mat, bool omitScale)
    {
        Vector3    loc;
        Quaternion rot;
        Vector3    scale;
        mat.Decompose(loc, rot, scale);
        SetLocation(loc);
        SetRotation(rot);

        if (!omitScale)
            SetScale(scale);
    }

    void EntityDataComponent::SetLocalTransformation(Matrix& mat, bool omitScale)
    {
        Vector3    loc;
        Quaternion rot;
        Vector3    scale;
        mat.Decompose(loc, rot, scale);
        SetLocalLocation(loc);
        SetLocalRotation(rot);

        if (!omitScale)
            SetLocalScale(scale);
    }

    void EntityDataComponent::AddRotation(const Vector3& angles)
    {
        if (m_isTransformLocked)
            return;
        SetRotationAngles(GetRotationAngles() + angles);
    }

    void EntityDataComponent::AddLocaRotation(const Vector3& angles)
    {
        if (m_isTransformLocked)
            return;
        SetLocalRotationAngles(GetLocalRotationAngles() + angles);
    }

    void EntityDataComponent::AddLocation(const Vector3& loc)
    {
        if (m_isTransformLocked)
            return;
        SetLocation(GetLocation() + loc);
    }

    void EntityDataComponent::AddLocalLocation(const Vector3& loc)
    {
        if (m_isTransformLocked)
            return;
        SetLocalLocation(GetLocalLocation() + loc);
    }

    Transformation EntityDataComponent::GetInterpolated(float interpolation)
    {
        Transformation t;
        t.m_location = Vector3::Lerp(m_transform.m_previousLocation, m_transform.m_location, interpolation);
        t.m_scale    = Vector3::Lerp(m_transform.m_previousScale, m_transform.m_scale, interpolation);
        t.m_rotation = Quaternion::Euler(Vector3::Lerp(m_transform.m_previousAngles, m_transform.m_rotation.GetEuler(), interpolation));
        return t;
    }

    void EntityDataComponent::SetLocalLocation(const Vector3& loc)
    {
        if (m_isTransformLocked)
            return;
        m_transform.m_localLocation = loc;
        UpdateGlobalLocation();

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalLocation();
        }
    }
    void EntityDataComponent::SetLocation(const Vector3& loc)
    {
        if (m_isTransformLocked)
            return;
        m_transform.m_previousLocation = m_transform.m_location;
        m_transform.m_location         = loc;
        UpdateLocalLocation();

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalLocation();
        }
    }

    void EntityDataComponent::SetLocalRotation(const Quaternion& rot, bool isThisPivot)
    {
        if (m_isTransformLocked)
            return;
        m_transform.m_localRotation       = rot;
        m_transform.m_localRotationAngles = rot.GetEuler();
        UpdateGlobalRotation();

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalRotation();

            if (isThisPivot)
                d.UpdateGlobalLocation();
        }
    }

    void EntityDataComponent::SetLocalRotationAngles(const Vector3& angles, bool isThisPivot)
    {
        if (m_isTransformLocked)
            return;
        m_transform.m_localRotationAngles = angles;
        m_transform.m_localRotation       = Quaternion::FromVector(glm::radians((glm::vec3)angles));
        UpdateGlobalRotation();

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalRotation();

            if (isThisPivot)
                d.UpdateGlobalLocation();
        }
    }

    void EntityDataComponent::SetRotation(const Quaternion& rot, bool isThisPivot)
    {
        if (m_isTransformLocked)
            return;
        m_transform.m_previousAngles = m_transform.m_rotationAngles;
        m_transform.m_rotation       = rot;
        m_transform.m_rotationAngles = rot.GetEuler();
        UpdateLocalRotation();

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalRotation();

            if (isThisPivot)
                d.UpdateGlobalLocation();
        }
    }

    void EntityDataComponent::SetRotationAngles(const Vector3& angles, bool isThisPivot)
    {
        if (m_isTransformLocked)
            return;
        m_transform.m_previousAngles = m_transform.m_rotationAngles;
        m_transform.m_rotationAngles = angles;
        m_transform.m_rotation       = Quaternion::FromVector(glm::radians((glm::vec3)angles));
        UpdateLocalRotation();

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalRotation();

            if (isThisPivot)
                d.UpdateGlobalLocation();
        }
    }

    void EntityDataComponent::SetLocalScale(const Vector3& scale, bool isThisPivot)
    {
        if (m_isTransformLocked)
            return;
        m_transform.m_localScale = scale;
        UpdateGlobalScale();

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalScale();

            if (isThisPivot)
                d.UpdateGlobalLocation();
        }
    }

    void EntityDataComponent::SetScale(const Vector3& scale, bool isThisPivot)
    {
        if (m_isTransformLocked)
            return;
        m_transform.m_previousScale = m_transform.m_scale;
        m_transform.m_scale         = scale;
        UpdateLocalScale();

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalScale();

            if (isThisPivot)
                d.UpdateGlobalLocation();
        }
    }

    void EntityDataComponent::UpdateGlobalLocation()
    {
        if (m_isTransformLocked)
            return;

        if (m_parent == entt::null)
        {
            m_transform.m_previousLocation = m_transform.m_location;
            m_transform.m_location         = m_transform.m_localLocation;
        }
        else
        {
            auto&   d                      = ECS::Registry::Get()->get<EntityDataComponent>(m_parent);
            Matrix  global                 = d.m_transform.ToMatrix() * m_transform.ToLocalMatrix();
            Vector3 translation            = global.GetTranslation();
            m_transform.m_previousLocation = m_transform.m_location;
            m_transform.m_location         = translation;
        }

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalLocation();
        }
    }

    void EntityDataComponent::UpdateLocalLocation()
    {
        if (m_isTransformLocked)
            return;

        if (m_parent == entt::null)
            m_transform.m_localLocation = m_transform.m_location;
        else
        {
            auto&  d                    = ECS::Registry::Get()->get<EntityDataComponent>(m_parent);
            Matrix global               = d.m_transform.ToMatrix().Inverse() * m_transform.ToMatrix();
            m_transform.m_localLocation = global.GetTranslation();
        }
    }

    void EntityDataComponent::UpdateGlobalScale()
    {
        if (m_isTransformLocked)
            return;

        if (m_parent == entt::null)
        {
            m_transform.m_previousScale = m_transform.m_scale;
            m_transform.m_scale         = m_transform.m_localScale;
        }
        else
        {
            auto&   d      = ECS::Registry::Get()->get<EntityDataComponent>(m_parent);
            Matrix  global = Matrix::Scale(d.m_transform.m_scale) * Matrix::Scale(m_transform.m_localScale);
            Vector3 scale  = global.GetScale();
            ;
            m_transform.m_previousScale = m_transform.m_scale;
            m_transform.m_scale         = scale;
        }

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalScale();
        }
    }

    void EntityDataComponent::UpdateGlobalRotation()
    {
        if (m_isTransformLocked)
            return;

        if (m_parent == entt::null)
        {
            m_transform.m_previousAngles = m_transform.m_rotationAngles;
            m_transform.m_rotation       = m_transform.m_localRotation;
            m_transform.m_rotationAngles = m_transform.m_localRotationAngles;
        }
        else
        {
            auto&      d      = ECS::Registry::Get()->get<EntityDataComponent>(m_parent);
            Matrix     global = Matrix::InitRotation(d.m_transform.m_rotation) * m_transform.ToLocalMatrix();
            Quaternion targetRot;
            global.Decompose(Vector3(), targetRot);
            m_transform.m_previousAngles = m_transform.m_rotationAngles;
            m_transform.m_rotation       = targetRot;
            m_transform.m_rotationAngles = m_transform.m_rotation.GetEuler();
        }

        for (auto child : m_children)
        {
            auto& d = ECS::Registry::Get()->get<EntityDataComponent>(child);
            d.UpdateGlobalRotation();
        }
    }

    void EntityDataComponent::UpdateLocalScale()
    {
        if (m_isTransformLocked)
            return;

        if (m_parent == entt::null)
            m_transform.m_localScale = m_transform.m_scale;
        else
        {
            auto&  d                 = ECS::Registry::Get()->get<EntityDataComponent>(m_parent);
            Matrix global            = Matrix::Scale(d.m_transform.m_scale).Inverse() * Matrix::Scale(m_transform.m_scale);
            m_transform.m_localScale = global.GetScale();
        }
    }

    void EntityDataComponent::UpdateLocalRotation()
    {
        if (m_isTransformLocked)
            return;

        if (m_parent == entt::null)
        {
            m_transform.m_localRotation       = m_transform.m_rotation;
            m_transform.m_localRotationAngles = m_transform.m_rotationAngles;
        }
        else
        {
            auto&  d      = ECS::Registry::Get()->get<EntityDataComponent>(m_parent);
            Matrix global = Matrix::InitRotation(d.m_transform.m_rotation).Inverse() * m_transform.ToMatrix();
            global.Decompose(Vector3(), m_transform.m_localRotation);
            m_transform.m_localRotationAngles = m_transform.m_localRotation.GetEuler();
        }
    }
} // namespace Lina::ECS
