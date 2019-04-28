/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Plane
Timestamp: 4/28/2019 2:06:43 AM

*/

#include "LinaPch.hpp"
#include "Utility/Math/Plane.hpp"  
#include "Utility/Math/Matrix.hpp"

namespace LinaEngine
{
	Plane Plane::transform(const Matrix& transform) const
	{
		return Plane(transform.toNormalMatrix().Transform(data)).normalized();
	}

	bool Plane::intersectPlanes(Vector3F& intersectionPoint, const Plane& other1, const Plane& other2, float errorMargin) const
	{
		Vector cross01 = data.Cross3(other1.data);
		float det = cross01.Dot3(other2.data)[0];
		float detSq = det * det;
		if (detSq < errorMargin) {
			return false;
		}
		Vector cross12 = other1.data.Cross3(other2.data);
		Vector cross20 = other2.data.Cross3(data);
		Vector w0 = data.Replicate(3);
		Vector w1 = other1.data.Replicate(3);
		Vector w2 = other2.data.Replicate(3);
		Vector invDet = Vector::Load1F(-Math::Reciprocal(det));

		intersectionPoint = Vector3F(invDet*(w0*cross12 + w1 * cross20 + w2 * cross01));
		return true;
	}
}

