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

#ifndef Frustum_HPP
#define Frustum_HPP

// Headers here.
#include "Common/Math/Plane.hpp"

namespace Lina
{
	class Matrix4;
	class AABB;

	enum class FrustumTest
	{
		Inside,
		Outside,
		Intersects
	};

	class Frustum
	{

	public:
		Frustum()  = default;
		~Frustum() = default;

		/// <summary>
		/// Extract planes from the given matrix.
		/// Projection Matrix = results in eye-space
		/// View*Projection Matrix = results in world-space
		/// Model*View*Projection Matrix = results in model-space
		/// </summary>
		void Calculate(const Matrix4& matrix, bool normalizeAll);

		/// <summary>
		/// Expects the AABB's bounds to be already transformed according to a world position.
		/// </summary>
		/// <param name="aabb"></param>
		/// <returns></returns>
		FrustumTest TestIntersection(const AABB& aabb) const;

	private:
		Plane m_left;
		Plane m_right;
		Plane m_bottom;
		Plane m_top;
		Plane m_near;
		Plane m_far;
	};
} // namespace Lina

#endif
