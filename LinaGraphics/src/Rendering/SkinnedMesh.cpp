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
Timestamp: 6/2/2020 10:04:48 PM

*/

#include "Rendering/SkinnedMesh.hpp"

namespace LinaEngine::Graphics
{
	void SkinnedMesh::Construct(Joint rootJoint, int jointCount)
	{
		m_RootJoint = rootJoint;
		m_JointCount = jointCount;
		m_RootJoint.CalculateInverseBindTransform(Matrix::Identity());
	}
	void SkinnedMesh::PlayAnimation(Animation animation)
	{
		// Play animation.
		m_Animator.PlayAnimation(animation);
	}
	void SkinnedMesh::Update(float delta)
	{
		m_Animator.Update(delta);
	}
	void SkinnedMesh::AddJointsToList(Joint head, std::map<int, Matrix>& jointMatrices)
	{
		jointMatrices[head.GetID()] = head.GetAnimatedTransform();
		for (auto& j : head.GetChildren())
			AddJointsToList(j, jointMatrices);
	}

	std::map<int, Matrix> SkinnedMesh::GetJointTransforms()
	{
		std::map<int, Matrix> jointMatrices;
		AddJointsToList(m_RootJoint, jointMatrices);
		return jointMatrices;
	}
}