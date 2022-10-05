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

#include "Core/Entity.hpp"
#include "Core/Component.hpp"
#include "Core/World.hpp"
#include "EventSystem/EntityEvents.hpp"
#include "EventSystem/EventSystem.hpp"

namespace Lina::World
{
    void Entity::AddChild(Entity* e)
    {
        m_childrenID.insert(e->m_id);
        m_children.insert(e);
        e->m_parentID = m_id;
        e->m_parent   = this;
    }

    void Entity::RemoveChild(Entity* e)
    {
        m_childrenID.erase(e->m_id);
        m_children.erase(e);
        e->m_parentID = ENTITY_NULL;
        e->m_parent   = nullptr;
    }

    void Entity::RemoveFromParent()
    {
        if (m_parentID != ENTITY_NULL)
            m_parent->RemoveChild(this);
    }

    void Entity::SetTransformation(Matrix& mat, bool omitScale)
    {
        Vector3    loc;
        Quaternion rot;
        Vector3    scale;
        mat.Decompose(loc, rot, scale);
        SetPosition(loc);
        SetRotation(rot);

        if (!omitScale)
            SetScale(scale);
    }

    void Entity::SetLocalTransformation(Matrix& mat, bool omitScale)
    {
        Vector3    loc;
        Quaternion rot;
        Vector3    scale;
        mat.Decompose(loc, rot, scale);
        SetLocalPosition(loc);
        SetLocalRotation(rot);

        if (!omitScale)
            SetLocalScale(scale);
    }

    void Entity::AddRotation(const Vector3& angles)
    {
        SetRotationAngles(GetRotationAngles() + angles);
    }

    void Entity::AddLocalRotation(const Vector3& angles)
    {
        SetLocalRotationAngles(GetLocalRotationAngles() + angles);
    }

    void Entity::AddPosition(const Vector3& loc)
    {
        SetPosition(GetPosition() + loc);
    }

    void Entity::AddLocalPosition(const Vector3& loc)
    {
        SetLocalPosition(GetLocalPosition() + loc);
    }

    Transformation Entity::GetInterpolated(float interpolation)
    {
        Transformation t;
        t.m_position = Vector3::Lerp(m_transform.m_previousPosition, m_transform.m_position, interpolation);
        t.m_scale    = Vector3::Lerp(m_transform.m_previousScale, m_transform.m_scale, interpolation);
        t.m_rotation = Quaternion::Euler(Vector3::Lerp(m_transform.m_previousAngles, m_transform.m_rotation.GetEuler(), interpolation));
        return t;
    }

    void Entity::SetLocalPosition(const Vector3& loc)
    {
        m_transform.m_localPosition = loc;
        UpdateGlobalPosition();

        for (auto child : m_children)
            child->UpdateGlobalPosition();
    }
    void Entity::SetPosition(const Vector3& loc)
    {
        m_transform.m_previousPosition = m_transform.m_position;
        m_transform.m_position         = loc;
        UpdateLocalPosition();

        for (auto child : m_children)
            child->UpdateGlobalPosition();
    }

    void Entity::SetLocalRotation(const Quaternion& rot, bool isThisPivot)
    {
        m_transform.m_localRotation       = rot;
        m_transform.m_localRotationAngles = rot.GetEuler();
        UpdateGlobalRotation();

        for (auto child : m_children)
        {
            child->UpdateGlobalRotation();

            if (isThisPivot)
                child->UpdateGlobalPosition();
        }
    }

    void Entity::SetLocalRotationAngles(const Vector3& angles, bool isThisPivot)
    {
        m_transform.m_localRotationAngles = angles;
        const Vector3 vang                = glm::radians(static_cast<glm::vec3>(angles));
        m_transform.m_localRotation       = Quaternion::FromVector(vang);
        UpdateGlobalRotation();

        for (auto child : m_children)
        {
            child->UpdateGlobalRotation();

            if (isThisPivot)
                child->UpdateGlobalPosition();
        }
    }

    void Entity::SetRotation(const Quaternion& rot, bool isThisPivot)
    {
        m_transform.m_previousAngles = m_transform.m_rotationAngles;
        m_transform.m_rotation       = rot;
        m_transform.m_rotationAngles = rot.GetEuler();
        UpdateLocalRotation();

        for (auto child : m_children)
        {
            child->UpdateGlobalRotation();

            if (isThisPivot)
                child->UpdateGlobalPosition();
        }
    }

    void Entity::SetRotationAngles(const Vector3& angles, bool isThisPivot)
    {
        m_transform.m_previousAngles = m_transform.m_rotationAngles;
        m_transform.m_rotationAngles = angles;
        m_transform.m_rotation       = Quaternion::FromVector(glm::radians(static_cast<glm::vec3>(angles)));
        UpdateLocalRotation();

        for (auto child : m_children)
        {
            child->UpdateGlobalRotation();

            if (isThisPivot)
                child->UpdateGlobalPosition();
        }
    }

    void Entity::SetLocalScale(const Vector3& scale, bool isThisPivot)
    {
        m_transform.m_localScale = scale;
        UpdateGlobalScale();

        for (auto child : m_children)
        {
            child->UpdateGlobalScale();

            if (isThisPivot)
                child->UpdateGlobalPosition();
        }
    }

    void Entity::SetScale(const Vector3& scale, bool isThisPivot)
    {
        m_transform.m_previousScale = m_transform.m_scale;
        m_transform.m_scale         = scale;
        UpdateLocalScale();

        for (auto child : m_children)
        {
            child->UpdateGlobalScale();

            if (isThisPivot)
                child->UpdateGlobalPosition();
        }
    }

    void Entity::UpdateGlobalPosition()
    {
        if (m_parent == nullptr)
        {
            m_transform.m_previousPosition = m_transform.m_position;
            m_transform.m_position         = m_transform.m_localPosition;
        }
        else
        {
            Matrix  global                 = m_parent->m_transform.ToMatrix() * m_transform.ToLocalMatrix();
            Vector3 translation            = global.GetTranslation();
            m_transform.m_previousPosition = m_transform.m_position;
            m_transform.m_position         = translation;
        }

        for (auto child : m_children)
        {
            child->UpdateGlobalPosition();
        }
    }

    void Entity::UpdateLocalPosition()
    {
        if (m_parent == nullptr)
            m_transform.m_localPosition = m_transform.m_position;
        else
        {
            Matrix global               = m_parent->m_transform.ToMatrix().Inverse() * m_transform.ToMatrix();
            m_transform.m_localPosition = global.GetTranslation();
        }
    }

    void Entity::UpdateGlobalScale()
    {
        if (m_parent == nullptr)
        {
            m_transform.m_previousScale = m_transform.m_scale;
            m_transform.m_scale         = m_transform.m_localScale;
        }
        else
        {
            Matrix  global = Matrix::Scale(m_parent->m_transform.m_scale) * Matrix::Scale(m_transform.m_localScale);
            Vector3 scale  = global.GetScale();
            ;
            m_transform.m_previousScale = m_transform.m_scale;
            m_transform.m_scale         = scale;
        }

        for (auto child : m_children)
        {
            child->UpdateGlobalScale();
        }
    }

    void Entity::UpdateGlobalRotation()
    {
        if (m_parent == nullptr)
        {
            m_transform.m_previousAngles = m_transform.m_rotationAngles;
            m_transform.m_rotation       = m_transform.m_localRotation;
            m_transform.m_rotationAngles = m_transform.m_localRotationAngles;
        }
        else
        {
            Matrix     global = Matrix::InitRotation(m_parent->m_transform.m_rotation) * m_transform.ToLocalMatrix();
            Quaternion targetRot;
            Vector3    s = Vector3(), p = Vector3();
            global.Decompose(p, targetRot, s);
            m_transform.m_previousAngles = m_transform.m_rotationAngles;
            m_transform.m_rotation       = targetRot;
            m_transform.m_rotationAngles = m_transform.m_rotation.GetEuler();
        }

        for (auto child : m_children)
        {
            child->UpdateGlobalRotation();
        }
    }

    void Entity::UpdateLocalScale()
    {

        if (m_parent == nullptr)
            m_transform.m_localScale = m_transform.m_scale;
        else
        {
            Matrix global            = Matrix::Scale(m_parent->m_transform.m_scale).Inverse() * Matrix::Scale(m_transform.m_scale);
            m_transform.m_localScale = global.GetScale();
        }
    }

    void Entity::UpdateLocalRotation()
    {
        if (m_parent == nullptr)
        {
            m_transform.m_localRotation       = m_transform.m_rotation;
            m_transform.m_localRotationAngles = m_transform.m_rotationAngles;
        }
        else
        {
            Matrix  global = Matrix::InitRotation(m_parent->m_transform.m_rotation).Inverse() * m_transform.ToMatrix();
            Vector3 s = Vector3(), p = Vector3();
            global.Decompose(s, m_transform.m_localRotation, p);
            m_transform.m_localRotationAngles = m_transform.m_localRotation.GetEuler();
        }
    }

    void Entity::SetVisible(bool visible)
    {
        Event::EEntityMaskVisibilityChanged ev;
        ev.entity     = this;
        ev.wasVisible = IsVisible();

        if (visible)
            m_mask.Set(EntityMask::Visible);
        else
            m_mask.Remove(EntityMask::Visible);

        for (auto c : m_children)
            c->SetVisible(visible);

        ev.isVisible = IsVisible();
        Event::EventSystem::Get()->Trigger<Event::EEntityMaskVisibilityChanged>(ev);
    }

    void Entity::SetStatic(bool isStatic)
    {
        Event::EEntityMaskStaticChanged ev;
        ev.entity    = this;
        ev.wasStatic = IsStatic();

        if (isStatic)
            m_mask.Set(EntityMask::Static);
        else
            m_mask.Remove(EntityMask::Static);

        for (auto c : m_children)
            c->SetStatic(isStatic);

        ev.isStatic = IsStatic();
        Event::EventSystem::Get()->Trigger<Event::EEntityMaskStaticChanged>(ev);
    }
} // namespace Lina::World
