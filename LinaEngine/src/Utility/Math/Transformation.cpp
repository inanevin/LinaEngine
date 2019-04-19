/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Transformation
Timestamp: 4/9/2019 12:06:04 PM

*/

#include "LinaPch.hpp"
#include "Utility/Math/Transformation.hpp"  

namespace LinaEngine
{
	Matrix4F Transformation::Inverse() const
	{
		// NOTE: If scales are entirely uniform, then this can always be represented
		// by another transform, calculated like this. However, in the case of
		// non-uniform scaling, the inverse matrix is not guaranteed to fit the
		// format of the transform class. Hence, the transform's inverse must be
		// returned in matrix form, NOT as another transform.
	//	Vector3f invScale = scale.reciprocal();
	//	Quaternion invRotation = rotation.inverse();
	//	Vector3f invTranslation = invRotation.rotate(invScale*-translation);
	//	return Transform(invTranslation, invRotation, invScale);

		// TODO: Use the special fast matrix inverse for transform matrices. There's an article
		// about it in my bookmarks
		Matrix4F Inverse(ToMatrix().Inverse());
		return Inverse;
	}
}

