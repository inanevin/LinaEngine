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

Class: Animator
Timestamp: 6/3/2020 1:52:31 PM

*/
#pragma once

#ifndef Animator_HPP
#define Animator_HPP

#include "Rendering/SkinnedMesh.hpp"
#include "Animation.hpp"

namespace LinaEngine::Graphics
{
	class Animator
	{
		
	public:
		
		Animator() {};
		Animator(SkinnedMesh mesh) { m_Mesh = mesh; }
		~Animator() {};
	
		FORCEINLINE void PlayAnimation(Animation animation)
		{
			m_AnimationTime = 0.0f;
			m_CurrentAnimation = animation;
		}

		void Update(float delta);
		void IncreaseAnimationTime(float delta);
		std::map<std::string, Matrix> CalculateCurrentAnimationPose();
		void ApplyPoseToJoints(std::map<std::string, Matrix> currentPose, Joint joint, Matrix parentTransform);
		std::vector<KeyFrame> GetPreviousAndNextFrames();
		float CalculateProgression(KeyFrame previousFrame, KeyFrame nextFrame);
		std::map<std::string, Matrix> InterpolatePoses(KeyFrame previousFrame, KeyFrame nextFrame, float progression);

	private:
	
		SkinnedMesh m_Mesh;
		Animation m_CurrentAnimation;
		float m_AnimationTime = 0.0f;
	};
}

#endif
