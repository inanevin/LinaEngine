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

#ifndef Entity_HPP
#define Entity_HPP

#include "Core/SizeDefinitions.hpp"
#include "Data/String.hpp"
#include "Serialization/SetSerialization.hpp"
#include "Math/Transformation.hpp"
#include "WorldCommon.hpp"

namespace Lina
{
    namespace Memory
    {
        class MemoryManager;
    }
} // namespace Lina

namespace Lina::World
{

    // Actual game state
    class Entity
    {
    public:
        Entity()  = default;
        ~Entity() = default;

        template <class Archive> void Serialize(Archive& archive)
        {
            archive(m_id, m_name, m_parentID, m_childrenID, m_transform, m_mask);
        }

        inline uint32 GetID()
        {
            return m_id;
        }

        Matrix ToMatrix() const
        {
            return m_transform.ToMatrix();
        }
        Matrix ToLocalMatrix() const
        {
            return m_transform.ToLocalMatrix();
        }

        void           AddChild(Entity* e);
        void           RemoveChild(Entity* e);
        void           RemoveFromParent();
        void           SetTransformation(Matrix& mat, bool omitScale = false);
        void           SetLocalTransformation(Matrix& mat, bool omitScale = false);
        void           AddRotation(const Vector3& angles);
        void           AddLocalRotation(const Vector3& angles);
        void           AddPosition(const Vector3& loc);
        void           AddLocalPosition(const Vector3& loc);
        Transformation GetInterpolated(float interpolation);
        void           SetLocalPosition(const Vector3& loc);
        void           SetPosition(const Vector3& loc);
        void           SetLocalRotation(const Quaternion& rot, bool isThisPivot = true);
        void           SetLocalRotationAngles(const Vector3& angles, bool isThisPivot = true);
        void           SetRotation(const Quaternion& rot, bool isThisPivot = true);
        void           SetRotationAngles(const Vector3& angles, bool isThisPivot = true);
        void           SetLocalScale(const Vector3& scale, bool isThisPivot = true);
        void           SetScale(const Vector3& scale, bool isThisPivot = true);
        void           SetVisible(bool visible);
        void           SetStatic(bool isStatic);

        inline bool IsVisible()
        {
            return m_mask.IsSet(EntityMask::Visible);
        }

        inline bool IsStatic()
        {
            return m_mask.IsSet(EntityMask::Static);
        }

        const Vector3& GetLocalRotationAngles()
        {
            return m_transform.m_localRotationAngles;
        }
        const Vector3& GetLocalPosition()
        {
            return m_transform.m_localPosition;
        }
        const Quaternion& GetLocalRotation()
        {
            return m_transform.m_localRotation;
        }
        const Vector3& GetLocalScale()
        {
            return m_transform.m_localScale;
        }
        const Vector3& GetPosition()
        {
            return m_transform.m_position;
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

        const Transformation& GetTransform()
        {
            return m_transform;
        }

        inline void SetName(const String& name)
        {
            m_name = name;
        }

        inline bool IsMaskSet(uint16 mask)
        {
            return m_mask.IsSet(mask);
        }

        inline Bitmask16 GetEntityMask()
        {
            return m_mask;
        }

    private:
        void UpdateGlobalPosition();
        void UpdateLocalPosition();
        void UpdateGlobalRotation();
        void UpdateLocalRotation();
        void UpdateGlobalScale();
        void UpdateLocalScale();

    private:
        friend class EntityWorld;
        friend class Memory::MemoryManager;
        friend struct EntitySerializer;

        EntityWorld*     m_world          = nullptr;
        uint32           m_id             = 0;
        uint32           m_allocPoolIndex = 0;
        String           m_name           = "";
        Entity*          m_parent         = nullptr;
        HashSet<Entity*> m_children;
        uint32           m_parentID = ENTITY_NULL;
        HashSet<uint32>  m_childrenID;
        Transformation   m_transform;
        Bitmask16        m_mask;

    private:
    };

} // namespace Lina::World

#endif
