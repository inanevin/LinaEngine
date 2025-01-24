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

#include "Core/World/Entity.hpp"
#include "Core/World/Component.hpp"
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	void Entity::AddAABB(const AABB& aabb)
	{
		m_totalAABB.boundsMax = m_totalAABB.boundsMax.Max(aabb.boundsMax);
		m_totalAABB.boundsMin = m_totalAABB.boundsMin.Min(aabb.boundsMin);
	}

	void Entity::RemoveAABB(const AABB& aabb)
	{
	}

	Entity* Entity::FindInChildHierarchy(EntityID id)
	{
		for (Entity* c : m_children)
		{
			if (c->GetGUID() == id)
				return c;

			Entity* found = c->FindInChildHierarchy(id);

			if (found)
				return found;
		}

		return nullptr;
	}

	void Entity::AddChild(Entity* e)
	{
		e->RemoveFromParent();
		m_children.push_back(e);
		e->m_parent = this;
		e->UpdateLocalPosition();
		e->UpdateLocalRotation();
		e->UpdateLocalScale();
	}

	void Entity::RemoveChild(Entity* e)
	{
		m_children.erase(linatl::find_if(m_children.begin(), m_children.end(), [e](Entity* c) { return e == c; }));
		e->m_parent = nullptr;
	}

	void Entity::RemoveFromParent()
	{
		if (m_parent != nullptr)
			m_parent->RemoveChild(this);

		UpdateLocalPosition();
		UpdateLocalRotation();
		UpdateLocalScale();
	}

	void Entity::SetTransformation(const Matrix4& mat, bool omitScale)
	{
		Vector3	   loc;
		Quaternion rot;
		Vector3	   scale;
		mat.Decompose(loc, rot, scale);
		SetPosition(loc);
		SetRotation(rot);

		if (!omitScale)
			SetScale(scale);
	}

	void Entity::SetLocalTransformation(const Matrix4& mat, bool omitScale)
	{
		Vector3	   loc;
		Quaternion rot;
		Vector3	   scale;
		mat.Decompose(loc, rot, scale);
		SetLocalPosition(loc);
		SetLocalRotation(rot);

		if (!omitScale)
			SetLocalScale(scale);
	}

	void Entity::SetTransform(const Transformation& transform)
	{
		const Matrix4 mat = transform.ToMatrix();
		SetTransformation(mat);
		// m_transform = transform;
		// m_transform.UpdateGlobalMatrix();
		// m_transform.UpdateLocalMatrix();
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

	bool Entity::HasChildInTree(Entity* other) const
	{
		auto it = linatl::find_if(m_children.begin(), m_children.end(), [other](Entity* e) { return e == other || e->HasChildInTree(other); });
		return it != m_children.end();
	}

	void Entity::SetLocalPosition(const Vector3& loc)
	{
		m_transform.SetLocalPosition(loc);
		UpdateGlobalPosition();

		for (auto child : m_children)
			child->UpdateGlobalPosition();
	}
	void Entity::SetPosition(const Vector3& loc)
	{
		m_transform.SetPosition(loc);
		UpdateLocalPosition();

		for (auto child : m_children)
			child->UpdateGlobalPosition();
	}

	void Entity::SetLocalRotation(const Quaternion& rot, bool isThisPivot)
	{
		m_transform.SetLocalRotation(rot);
		m_transform.SetLocalRotationAngles(rot.GetPitchYawRoll());
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
		m_transform.SetLocalRotationAngles(angles);
		const Vector3 vang = glm::radians(static_cast<glm::vec3>(angles));
		m_transform.SetLocalRotation(Quaternion::FromVector(vang));
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
		m_transform.SetRotation(rot);
		m_transform.SetRotationAngles(rot.GetPitchYawRoll());
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
		m_transform.SetRotationAngles(angles);
		m_transform.SetRotation(Quaternion::FromVector(glm::radians(static_cast<glm::vec3>(angles))));
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
		m_transform.SetLocalScale(scale);
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
		m_transform.SetScale(scale);
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
			m_transform.SetPosition(m_transform.GetLocalPosition());
		else
		{
			Matrix4 global		= m_parent->m_transform.ToMatrix() * m_transform.ToLocalMatrix();
			Vector3 translation = global.GetTranslation();
			m_transform.SetPosition(translation);
		}

		for (auto child : m_children)
		{
			child->UpdateGlobalPosition();
		}
	}

	void Entity::UpdateLocalPosition()
	{
		if (m_parent == nullptr)
			m_transform.SetLocalPosition(m_transform.GetPosition());
		else
		{
			Matrix4 global = m_parent->m_transform.ToMatrix().Inverse() * m_transform.ToMatrix();
			m_transform.SetLocalPosition(global.GetTranslation());
		}
	}

	void Entity::UpdateGlobalScale()
	{
		if (m_parent == nullptr)
			m_transform.SetScale(m_transform.GetLocalScale());
		else
		{
			Matrix4 global = Matrix4::Scale(m_parent->m_transform.GetScale()) * Matrix4::Scale(m_transform.GetLocalScale());
			Vector3 scale  = global.GetScale();

			m_transform.SetScale(scale);
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
			m_transform.SetRotation(m_transform.GetLocalRotation());
			m_transform.SetRotationAngles(m_transform.GetLocalRotationAngles());
		}
		else
		{
			Matrix4	   global = Matrix4::InitRotation(m_parent->m_transform.GetRotation()) * m_transform.ToLocalMatrix();
			Quaternion targetRot;
			Vector3	   s = Vector3(), p = Vector3();
			global.Decompose(p, targetRot, s);
			m_transform.SetRotation(targetRot);
			m_transform.SetRotationAngles(m_transform.GetRotation().GetPitchYawRoll());
		}

		for (auto child : m_children)
		{
			child->UpdateGlobalRotation();
		}
	}

	void Entity::UpdateLocalScale()
	{
		if (m_parent == nullptr)
			m_transform.SetLocalScale(m_transform.GetScale());
		else
		{
			Matrix4 global = Matrix4::Scale(m_parent->m_transform.GetScale()).Inverse() * Matrix4::Scale(m_transform.GetScale());
			m_transform.SetLocalScale(global.GetScale());
		}
	}

	void Entity::SaveToStream(OStream& stream) const
	{
		stream << m_mask << m_transform << m_name << m_guid << m_physicsSettings << m_parameters;
	}

	void Entity::LoadFromStream(IStream& stream)
	{
		stream >> m_mask >> m_transform >> m_name >> m_guid >> m_physicsSettings >> m_parameters;
	}

	void Entity::UpdateLocalRotation()
	{
		if (m_parent == nullptr)
		{
			m_transform.SetLocalRotation(m_transform.GetRotation());
			m_transform.SetLocalRotationAngles(m_transform.GetRotationAngles());
		}
		else
		{
			Matrix4	   global = Matrix4::InitRotation(m_parent->m_transform.GetRotation()).Inverse() * m_transform.ToMatrix();
			Vector3	   s = Vector3(), p = Vector3();
			Quaternion q = {};
			global.Decompose(s, q, p);
			m_transform.SetLocalRotation(q);
			m_transform.SetLocalRotationAngles(m_transform.GetLocalRotation().GetPitchYawRoll());
		}
	}

	void Entity::SetVisible(bool isVisible)
	{
		m_mask.Set(EF_INVISIBLE, !isVisible);
		for (Entity* c : m_children)
			c->SetVisible(isVisible);
	}

	bool Entity::GetVisible() const
	{
		return !m_mask.IsSet(EF_INVISIBLE);
	}
	EntityParameter* Entity::GetParameter(StringID sid)
	{
		auto it = linatl::find_if(m_parameters.params.begin(),m_parameters.params.end(), [sid](EntityParameter& param) -> bool { return param._sid == sid; });
		if (it != m_parameters.params.end())
			return &(*it);
		return nullptr;
	}
} // namespace Lina
