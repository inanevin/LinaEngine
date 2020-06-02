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

Class: SkinnedMesh
Timestamp: 6/2/2020 8:11:34 PM

*/
#pragma once

#ifndef SkinnedMesh_HPP
#define SkinnedMesh_HPP

#include "Mesh.hpp"
#include "Animation/Joint.hpp"


namespace LinaEngine::Graphics
{
	class SkinnedMesh : public Mesh
	{
		
	public:
		
		SkinnedMesh() {};
		virtual ~SkinnedMesh() {  };

		void Construct(Joint rootJoint, int jointCount);
		void PlayAnimation(); // Gonna take in animation later on.
		void Update();
		void AddJointsToList(Joint head, std::map<int,Matrix>& jointMatrices);
		std::map<int, Matrix> GetJointTransforms();
		FORCEINLINE Joint& GetRootJoint() { return m_RootJoint; }
	private:

		// Animator m_Animator;
		Joint m_RootJoint;
		int m_JointCount;

	
	};
}

#endif
