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

#include "Utility/Math/Transformation.hpp"  
#include "Utility/Math/Math.hpp"

namespace LinaEngine
{
	Transformation Transformation::Interpolate(Transformation& from, Transformation& to, float t)
	{
		return Transformation(Vector3::Lerp(from.m_location, to.m_location, t), Quaternion::Slerp(from.m_rotation, to.m_rotation, t), Vector3::Lerp(from.m_scale, to.m_scale, t));
	}

	void Transformation::SetLocalLocation(const Vector3& loc)
	{
		m_localLocation = loc;

		UpdateGlobalLocation();

		for (Transformation* child : m_children)
			child->UpdateGlobalLocation();
	}

	void Transformation::SetLocalRotation(const Quaternion& rot, bool isThisPivot)
	{
		m_localRotation = rot;

		UpdateGlobalRotation();

		for (Transformation* child : m_children)
		{
			child->UpdateGlobalRotation();

			if (isThisPivot)
				child->UpdateGlobalLocation();
		}
	}

	void Transformation::SetLocalScale(const Vector3& scale, bool isThisPivot)
	{
		m_localScale = scale;

		UpdateGlobalScale();

		for (Transformation* child : m_children)
		{
			child->UpdateGlobalScale();
			
			if (isThisPivot)
				child->UpdateGlobalLocation();
		}
	}

	void Transformation::SetGlobalLocation(const Vector3& loc)
	{
		m_location = loc;

		for (Transformation* child : m_children)
		{
			child->SetLocalLocation(child->GetLocalLocation());
		}

		if (m_parent != nullptr)
		{
			Matrix local = m_parent->ToMatrix().Inverse() * Matrix::Translate(m_location);
			local.Decompose(m_localLocation);
		}
		else
			m_localLocation = loc;
	}

	void Transformation::SetGlobalRotation(const Quaternion& rot)
	{
		m_rotation = rot;

		for (Transformation* child : m_children)
		{
			child->SetLocalRotation(child->GetLocalRotation());
		}

		if (m_parent != nullptr)
		{
			Matrix local = m_parent->ToMatrix().Inverse() * Matrix::TransformMatrix(m_location, m_rotation, m_scale);
			local.Decompose(m_localLocation, m_localRotation);
		}
		else
			m_localRotation = rot;
	}

	void Transformation::SetGlobalScale(const Vector3& scale)
	{
		m_scale = scale;

		for (Transformation* child : m_children)
		{
			child->SetLocalScale(child->GetLocalScale());
		}

		if (m_parent != nullptr)
		{
			Matrix local = m_parent->ToMatrix().Inverse() * Matrix::TransformMatrix(m_location, m_rotation, m_scale);
			local.Decompose(Vector3(), Quaternion(), m_localScale);
		}
		else
			m_localScale = scale;
	}

	void Transformation::AddChild(Transformation* child)
	{
		if (child != this)
		{
			m_children.emplace(child);
			child->m_parent = this;
		}
		else
			LINA_CORE_WARN("You can not add a transformation as it's own child.");
	}

	void Transformation::RemoveChild(Transformation* child)
	{
		if (m_children.find(child) != m_children.end())
		{
			child->m_parent = nullptr;
			m_children.erase(child);
		}
		else
			LINA_CORE_WARN("Child not found, can't remove.");
	}

	void Transformation::RemoveFromParent()
	{
		if (m_parent != nullptr)
			m_parent->RemoveChild(this);
		else
			LINA_CORE_WARN("This transformation doesn't have a parent to remove from.");

	}

	void Transformation::UpdateGlobalScale()
	{
		if (m_parent == nullptr)
			m_scale = m_localScale;
		else
		{
			Matrix global = Matrix::Scale(m_parent->m_scale) * Matrix::Scale(m_localScale);
			m_scale = global.GetScale();
		}
	}

	void Transformation::UpdateGlobalLocation()
	{
		if (m_parent == nullptr)
			m_location = m_localLocation;
		else
		{
			Matrix global = m_parent->ToMatrix() * ToLocalMatrix();
			m_location = global.GetTranslation();
		}
	}

	void Transformation::UpdateGlobalRotation()
	{
		if (m_parent == nullptr)
			m_rotation = m_localRotation;
		else
		{
			Matrix global = Matrix::InitRotation(m_parent->m_rotation) * ToLocalMatrix();
			global.Decompose(Vector3(), m_rotation);
		}
	}

	void Transformation::UpdateLocalScale()
	{
		if (m_parent == nullptr)
			m_scale = m_localScale;
		else
		{
			Matrix global = Matrix::Scale(m_parent->m_scale).Inverse() * Matrix::Scale(m_localScale);
			m_scale = global.GetScale();
		}
	}

	void Transformation::UpdateLocalLocation()
	{
		if (m_parent == nullptr)
			m_location = m_localLocation;
		else
		{
			Matrix global = m_parent->ToMatrix() * ToLocalMatrix();
			m_location = global.GetTranslation();
		}
	}

	void Transformation::UpdateLocalRotation()
	{
		if (m_parent == nullptr)
			m_rotation = m_localRotation;
		else
		{
			Matrix global = m_parent->ToMatrix() * ToLocalMatrix();
			global.Decompose(Vector3(), m_rotation);
		}
	}
}

