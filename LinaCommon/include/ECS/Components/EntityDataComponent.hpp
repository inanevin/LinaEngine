/*
Class: EntityDataComponent



Timestamp: 12/8/2021 5:35:25 PM
*/

#pragma once

#ifndef EntityDataComponent_HPP
#define EntityDataComponent_HPP

// Headers here.
#include "Core/CommonECS.hpp"
#include "ECS/Component.hpp"
#include "Math/Transformation.hpp"

#include "Data/Serialization/SetSerialization.hpp"
#include "Data/Serialization/StringSerialization.hpp"

namespace Lina
{
    namespace Physics
    {
        class PhysicsEngine;
    } // namespace Physics
};    // namespace Lina

namespace Lina::ECS
{
    struct ModelRendererComponent;

    LINA_COMPONENT("Entity Data Component", "ICON_FA_DATABASE", "", "false", "false")
    struct EntityDataComponent : public Component
    {
        EntityDataComponent(bool enabled, bool serialized, String name)
        {
            m_isEnabled            = enabled;
            m_serialized           = serialized;
            m_name                 = name;
            m_wasPreviouslyEnabled = m_isEnabled;
        }

        EntityDataComponent() = default;

        bool        m_serialized = true;
        String      m_name       = "";
        Set<Entity> m_children;
        Entity      m_parent = entt::null;

        virtual void SetIsEnabled(bool isEnabled) override;
        /* TRANSFORM OPERATIONS */

        Matrix ToMatrix()
        {
            return m_transform.ToMatrix();
        }
        Matrix ToLocalMatrix()
        {
            return m_transform.ToLocalMatrix();
        }
        void SetTransformation(Matrix& mat, bool omitScale = false);
        void SetLocalTransformation(Matrix& mat, bool omitScale = false);

        void AddRotation(const Vector3& angles);
        void AddLocaRotation(const Vector3& angles);
        void AddLocation(const Vector3& loc);
        void AddLocalLocation(const Vector3& loc);

        Transformation GetInterpolated(float interpolation);
        void           SetLocalLocation(const Vector3& loc);
        void           SetLocation(const Vector3& loc);
        void           SetLocalRotation(const Quaternion& rot, bool isThisPivot = true);
        void           SetLocalRotationAngles(const Vector3& angles, bool isThisPivot = true);
        void           SetRotation(const Quaternion& rot, bool isThisPivot = true);
        void           SetRotationAngles(const Vector3& angles, bool isThisPivot = true);
        void           SetLocalScale(const Vector3& scale, bool isThisPivot = true);
        void           SetScale(const Vector3& scale, bool isThisPivot = true);

        const Vector3& GetLocalRotationAngles()
        {
            return m_transform.m_localRotationAngles;
        }
        const Vector3& GetLocalLocation()
        {
            return m_transform.m_localLocation;
        }
        const Quaternion& GetLocalRotation()
        {
            return m_transform.m_localRotation;
        }
        const Vector3& GetLocalScale()
        {
            return m_transform.m_localScale;
        }
        const Vector3& GetLocation()
        {
            return m_transform.m_location;
        }
        const Quaternion& GetRotation()
        {
            return m_transform.m_rotation;
        }
        const Vector3& GetRotationAngles()
        {
            return m_transform.m_rotationAngles;
        }
        const Vector3& GetScale()
        {
            return m_transform.m_scale;
        }

    private:
        void UpdateGlobalLocation();
        void UpdateLocalLocation();
        void UpdateGlobalRotation();
        void UpdateLocalRotation();
        void UpdateGlobalScale();
        void UpdateLocalScale();

    private:
        friend class cereal::access;
        friend class Registry;
        friend class Physics::PhysicsEngine;

        bool           m_isTransformLocked    = false;
        bool           m_wasPreviouslyEnabled = false;
        Transformation m_transform;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_transform, m_isTransformLocked, m_isEnabled, m_wasPreviouslyEnabled, m_name, m_parent, m_children);
        }
    };
} // namespace Lina::ECS

#endif
