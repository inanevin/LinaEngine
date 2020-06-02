/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: JointTransform
Timestamp: 6/3/2020 1:18:21 AM

*/

#include "Animation/JointTransformation.hpp"

namespace LinaEngine::Graphics
{
	JointTransformation JointTransformation::Interpolate(JointTransformation& frameA, JointTransformation& frameB, float t)
	{
		return JointTransformation(Vector3::Lerp(frameA.m_Position, frameB.m_Position, t), Quaternion::Slerp(frameA.m_Rotation, frameB.m_Rotation, t));
	}

	Matrix JointTransformation::GetLocalTransform()
	{
		Matrix matrix = Matrix::Identity();
		matrix = Matrix::Translate(m_Position);
		matrix = matrix * m_Rotation.ToMatrix();
	}
}