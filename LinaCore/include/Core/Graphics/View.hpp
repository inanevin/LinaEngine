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

#ifndef View_HPP
#define View_HPP

#include "Common/Math/Frustum.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Data/HashSet.hpp"
#include "Core/Graphics/Data/RenderData.hpp"

namespace Lina
{
	class View
	{
	public:
		bool IsVisible(const AABB& aabb) const;

		inline const Vector3& GetPos() const
		{
			return m_pos;
		}
		inline const Matrix4& GetView() const
		{
			return m_view;
		}

		inline const Matrix4& GetProj() const
		{
			return m_proj;
		}

		float GetNear() const
		{
			return m_near;
		}

		float GetFar() const
		{
			return m_far;
		}

		void Tick(const Vector3& pos, const Matrix4& view, const Matrix4& proj, float n, float f);

	private:
		Matrix4 m_view;
		Matrix4 m_proj;
		Vector3 m_pos	  = Vector3::Zero;
		Frustum m_frustum = Frustum();
		float	m_near	  = 0.0f;
		float	m_far	  = 0.0f;
	};
} // namespace Lina

#endif
