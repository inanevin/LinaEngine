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

#include "Common/Math/Transformation.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	Transformation Transformation::Interpolate(const Transformation& from, const Transformation& to, float t)
	{
		return Transformation(Vector3::Lerp(from.m_position, to.m_position, t), Quaternion::Slerp(from.m_rotation, to.m_rotation, t), Vector3::Lerp(from.m_scale, to.m_scale, t));
	}

	void Transformation::SetMatrix(Matrix4& mat)
	{
		mat.Decompose(m_position, m_rotation, m_scale);
	}

	void Transformation::UpdateGlobalMatrix()
	{
		m_matrix	  = ToMatrix();
		m_localMatrix = ToLocalMatrix();
	}

	void Transformation::UpdateLocalMatrix()
	{
		m_localMatrix = ToLocalMatrix();
	}

	void Transformation::SaveToStream(OStream& stream)
	{
		m_position.SaveToStream(stream);
		m_rotation.SaveToStream(stream);
		m_scale.SaveToStream(stream);
		m_localPosition.SaveToStream(stream);
		m_localRotation.SaveToStream(stream);
		m_localScale.SaveToStream(stream);
		m_localRotationAngles.SaveToStream(stream);
		m_rotationAngles.SaveToStream(stream);
	}

	void Transformation::LoadFromStream(IStream& stream)
	{
		m_position.LoadFromStream(stream);
		m_rotation.LoadFromStream(stream);
		m_scale.LoadFromStream(stream);
		m_localPosition.LoadFromStream(stream);
		m_localRotation.LoadFromStream(stream);
		m_localScale.LoadFromStream(stream);
		m_localRotationAngles.LoadFromStream(stream);
		m_rotationAngles.LoadFromStream(stream);
	}
} // namespace Lina
