/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Transform
Timestamp: 1/5/2019 7:52:35 PM

*/

#include "LinaPch.hpp"
#include "Transform.hpp"  

namespace LinaEngine
{
	const Matrix4F & Transform::GetWorldTransformation()
	{
		Matrix4F translationM, rotationM, scaleM;

		scaleM.InitScaleTransform(scale.x, scale.y, scale.z);
		rotationM.InitRotationTransform(rotation.x, rotation.y, rotation.z);
		translationM.InitTranslationTransform(position.x, position.y, position.z);

		m_WorldTransformation = translationM * rotationM* scaleM;
		return m_WorldTransformation;
	}
}

