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
Timestamp: 4/9/2019 1:06:12 AM

*/

#include "LinaPch.hpp"
#include "VectorMath.hpp"  
#include <limits>

namespace LinaEngine
{
	const Vector VectorConstants::ZERO(Vector::Make(0.0f, 0.0f, 0.0f, 0.0f));
	const Vector VectorConstants::ONE(Vector::Make(1.0f, 1.0f, 1.0f, 1.0f));
	const Vector VectorConstants::TWO(Vector::Make(2.0f, 2.0f, 2.0f, 2.0f));
	const Vector VectorConstants::HALF(Vector::Make(0.5f, 0.5f, 0.5f, 0.5f));
	const Vector VectorConstants::INF(Vector::Make(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
	const Vector VectorConstants::MASK_X(Vector::Make((uint32)0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
	const Vector VectorConstants::MASK_Y(Vector::Make((uint32)0xFFFFFFFF, 0, 0xFFFFFFFF, 0xFFFFFFFF));
	const Vector VectorConstants::MASK_Z(Vector::Make((uint32)0xFFFFFFFF, 0xFFFFFFFF, 0, 0xFFFFFFFF));
	const Vector VectorConstants::MASK_W(Vector::Make((uint32)0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0));
	const Vector VectorConstants::SIGN_MASK(Vector::Make((uint32)0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF));
}

