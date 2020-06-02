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

Class: Joint
Timestamp: 6/2/2020 5:11:06 PM

*/
#pragma once

#ifndef Joint_HPP
#define Joint_HPP

#include "Utility/Math/Matrix.hpp"


namespace LinaEngine::Graphics
{
	class Joint
	{

	public:

		Joint() {};
		~Joint() {};

		FORCEINLINE void Construct(int id, std::string name, Matrix bindLocalTransform)
		{
			m_ID = id;
			m_Name = name;
			m_LocalBindTransform = bindLocalTransform;
		}

		FORCEINLINE void AddChild(Joint child) { m_Children.push_back(child); }
		FORCEINLINE Matrix& GetAnimatedTransform() { return m_AnimatedTransform; }
		FORCEINLINE void SetAnimationTransform(Matrix animationTransform) { m_AnimatedTransform = animationTransform; }
		FORCEINLINE Matrix GetInverseBindTransform() { return m_InverseBindTransform; }
		FORCEINLINE int GetID() { return m_ID; }
		FORCEINLINE std::vector<Joint>& GetChildren() { return m_Children; }
		void CalculateInverseBindTransform(Matrix parentBindTransform);

	private:

		int m_ID;
		std::string m_Name;
		std::vector<Joint> m_Children;
		Matrix m_AnimatedTransform;
		Matrix m_LocalBindTransform;
		Matrix m_InverseBindTransform;
	};
}

#endif
