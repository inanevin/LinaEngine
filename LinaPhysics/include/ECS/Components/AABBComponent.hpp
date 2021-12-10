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

/*
Class: AABB

Represents an AABB collision shape box.

Timestamp: 12/8/2020 4:52:07 PM
*/

#pragma once

#ifndef AABB_HPP
#define AABB_HPP

// Headers here.
#include "ECS/ECSComponent.hpp"
#include "Math/Vector.hpp"

namespace Lina::ECS
{
	struct AABBComponent : public ECSComponent
	{
		Lina::Vector3 m_center = Lina::Vector3::Zero;
		Lina::Vector3 m_halfWidths = Lina::Vector3::One;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_center, m_halfWidths);
		}
	};
}

#endif
