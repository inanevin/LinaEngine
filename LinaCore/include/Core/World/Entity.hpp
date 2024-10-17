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

#include "CommonWorld.hpp"
#include "Common/SizeDefinitions.hpp"
#include "Common/Math/Transformation.hpp"
#include "Common/Data/HashSet.hpp"
#include "Common/Memory/CommonMemory.hpp"
#include "Common/ObjectWrapper.hpp"
#include "Common/Math/AABB.hpp"

namespace Lina
{
	class EntityWorld;

	// Actual game state
	class Entity final
	{
	public:
		void AddChild(Entity* e);
		void RemoveChild(Entity* e);
		void RemoveFromParent();
		void SetTransformation(const Matrix4& mat, bool omitScale = false);
		void SetLocalTransformation(const Matrix4& mat, bool omitScale = false);
		void AddRotation(const Vector3& angles);
		void AddLocalRotation(const Vector3& angles);
		void AddPosition(const Vector3& loc);
		void AddLocalPosition(const Vector3& loc);
		void SetLocalPosition(const Vector3& loc);
		void SetPosition(const Vector3& loc);
		void SetLocalRotation(const Quaternion& rot, bool isThisPivot = true);
		void SetLocalRotationAngles(const Vector3& angles, bool isThisPivot = true);
		void SetRotation(const Quaternion& rot, bool isThisPivot = true);
		void SetRotationAngles(const Vector3& angles, bool isThisPivot = true);
		void SetLocalScale(const Vector3& scale, bool isThisPivot = true);
		void SetScale(const Vector3& scale, bool isThisPivot = true);
		bool HasChildInTree(Entity* other) const;
		void AddAABB(const AABB& aabb);
		void RemoveAABB(const AABB& aabb);

		void SaveToStream(OStream& stream);
		void LoadFromStream(IStream& stream);

		ObjectWrapper<EntityWorld> GetWorld()
		{
			return ObjectWrapper<EntityWorld>(m_world);
		}

		inline const String& GetName()
		{
			return m_name;
		}

		inline const Vector3& GetLocalRotationAngles() const
		{
			return m_transform.GetLocalRotationAngles();
		}
		inline const Vector3& GetLocalPosition() const
		{
			return m_transform.GetLocalPosition();
		}
		inline const Quaternion& GetLocalRotation() const
		{
			return m_transform.GetLocalRotation();
		}
		inline const Vector3& GetLocalScale() const
		{
			return m_transform.GetLocalScale();
		}
		inline const Vector3& GetPosition() const
		{
			return m_transform.GetPosition();
		}
		inline const Quaternion& GetRotation() const
		{
			return m_transform.GetRotation();
		}
		inline const Vector3& GetRotationAngles() const
		{
			return m_transform.GetRotationAngles();
		}
		inline const Vector3& GetScale() const
		{
			return m_transform.GetScale();
		}

		inline const Transformation& GetTransform() const
		{
			return m_transform;
		}

		inline void SetName(const String& name)
		{
			m_name = name;
		}

		inline Bitmask16 GetEntityFlags()
		{
			return m_mask;
		}

		inline const Vector<Entity*>& GetChildren() const
		{
			return m_children;
		}

		inline Entity* GetParent() const
		{
			return m_parent;
		}

	private:
		void UpdateGlobalPosition();
		void UpdateLocalPosition();
		void UpdateGlobalRotation();
		void UpdateLocalRotation();
		void UpdateGlobalScale();
		void UpdateLocalScale();

	private:
		friend class WorldRenderer;

	private:
		friend class EntityWorld;

		Entity()  = default;
		~Entity() = default;

		ALLOCATOR_BUCKET_MEM;
		ALLOCATOR_BUCKET_GET;

		EntityID		m_guid	 = 0;
		EntityWorld*	m_world	 = nullptr;
		Entity*			m_parent = nullptr;
		String			m_name	 = "";
		Vector<Entity*> m_children;
		Transformation	m_transform;
		Bitmask16		m_mask;
		AABB			m_totalAABB;
	};

} // namespace Lina
