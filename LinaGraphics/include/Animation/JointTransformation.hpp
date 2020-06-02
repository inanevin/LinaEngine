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

Class: JointTransformation
Timestamp: 6/3/2020 12:10:45 AM

*/
#pragma once

#ifndef JointTransformation_HPP
#define JointTransformation_HPP

#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Quaternion.hpp"
#include "Utility/Math/Matrix.hpp"


namespace LinaEngine::Graphics
{
	class JointTransformation
	{
		
	public:
		
		FORCEINLINE JointTransformation() {};
		FORCEINLINE JointTransformation(Vector3 pos, Quaternion rot) { m_Position = pos; m_Rotation = rot; }
		FORCEINLINE ~JointTransformation() {};

	protected:

		static JointTransformation Interpolate(JointTransformation& frameA, JointTransformation& frameB, float t);
		Matrix GetLocalTransform();

	private:
	
		Vector3 m_Position;
		Quaternion m_Rotation;
	};
}

#endif
