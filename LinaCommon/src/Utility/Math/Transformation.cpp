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

namespace LinaEngine
{
	Transformation Transformation::Interpolate(Transformation& from, Transformation& to, float t)
	{
		return Transformation(Vector3::Lerp(from.m_location, to.m_location, t), Quaternion::Slerp(from.m_rotation, to.m_rotation, t), Vector3::Lerp(from.m_scale, to.m_scale, t));
	}

	void Transformation::SetLocalLocation(const Vector3& loc)
	{
		m_localLocation = loc;
		Vector3 locationToSet = loc;

		if (m_parent != nullptr)
		{
			Matrix local = Matrix::Translate(m_localLocation);
			Matrix globalChild = m_parent->ToMatrix() * local;
			globalChild.Decompose(locationToSet);
		}

		SetGlobalLocation(locationToSet);
	}

	void Transformation::SetLocalRotation(const Quaternion& rot)
	{
		m_localRotation = rot;
		Quaternion rotationToSet = rot;
		Vector3 locationToSet = m_location;

		if (m_parent != nullptr)
		{
			Matrix local = Matrix::TransformMatrix(m_localLocation, m_localRotation, Vector3());
			Matrix globalChild = m_parent->ToMatrix() * local;
			globalChild.Decompose(locationToSet, rotationToSet);
		}

		SetGlobalLocation(locationToSet);
		SetGlobalRotation(rotationToSet);
	}

	void Transformation::SetLocalScale(const Vector3& scale)
	{
		m_localScale = scale;
		Vector3 scaleToSet = scale;

		if (m_parent != nullptr)
		{
			Matrix local = Matrix::Scale(m_localScale);
			Matrix globalChild = m_parent->ToMatrix() * local;
			globalChild.Decompose(Vector3(), Quaternion(), scaleToSet);
		}

		SetGlobalScale(scaleToSet);
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
	}

	void Transformation::SetGlobalRotation(const Quaternion& rot)
	{
		m_rotation = rot;

		for (Transformation* child : m_children)
		{
			child->SetLocalRotation(child->GetLocalRotation());
		}
	}

	void Transformation::SetGlobalScale(const Vector3& scale)
	{
		m_scale = scale;

		for (Transformation* child : m_children)
		{
			child->SetLocalScale(child->GetLocalScale());
		}
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
}

