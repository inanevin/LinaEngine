/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: VectorMath
Timestamp: 4/9/2019 1:03:58 AM

*/

#pragma once

#ifndef VectorMath_HPP
#define VectorMath_HPP


#include "Lina/PackageManager/PAMVectorMath.hpp"
#include "Math.hpp"
#include "Lina/Memory.hpp"

namespace LinaEngine
{

	typedef PAMVector Vector;

	struct VectorConstants
	{
	public:
		static const Vector ZERO;
		static const Vector ONE;
		static const Vector TWO;
		static const Vector HALF;
		static const Vector INF;
		static const Vector MASK_X;
		static const Vector MASK_Y;
		static const Vector MASK_Z;
		static const Vector MASK_W;
		static const Vector SIGN_MASK;
	};

	template<>
	inline Vector Math::Lerp(const Vector& val1, const Vector& val2,
		const float& amt)
	{
		return (val2 - val1) * Vector::Load1F(amt) + val1;
	}
}


#endif