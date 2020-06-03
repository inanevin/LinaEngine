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
Timestamp: 6/3/2020 1:52:45 PM

*/

#include "Animation/Animator.hpp"

namespace LinaEngine::Graphics
{
	void Animator::Update()
	{
		// IF Current anim is null
		IncreaseAnimationTime();
		std::map<std::string, Matrix> currentPose = CalculateCurrentAnimationPose();
		ApplyPoseToJoints(currentPose, m_Mesh.GetRootJoint(), Matrix());
	}

	void Animator::IncreaseAnimationTime(float delta)
	{
		m_AnimationTime += delta;
		if (m_AnimationTime > m_CurrentAnimation.GetLength())
			m_AnimationTime = (int)m_AnimationTime % (int)m_CurrentAnimation.GetLength();
	}

	std::map<std::string, Matrix> Animator::CalculateCurrentAnimationPose()
	{
		std::vector<KeyFrame> frames = GetPreviousAndNextFrames();
		float prog = CalculateProgression(frames[0], frames[1]);
		return InterpolatePoses(frames[0], frames[1], prog);
	}
	void Animator::ApplyPoseToJoints(std::map<std::string, Matrix> currentPose, Joint joint, Matrix parentTransform)
	{
		Matrix currentLocalTransform = currentPose[joint.GetName()];
		Matrix currentTransform = parentTransform * currentLocalTransform;
		for (auto& j : joint.GetChildren())
		{
			ApplyPoseToJoints(currentPose, j, currentTransform);
		}

		currentTransform = currentTransform * joint.GetInverseBindTransform(); // CHECK THIS
		joint.SetAnimationTransform(currentTransform);
	}
	std::vector<KeyFrame> Animator::GetPreviousAndNextFrames()
	{
		std::vector<KeyFrame> allFrames = m_CurrentAnimation.GetKeyFrames();
		KeyFrame previousFrame = allFrames[0];
		KeyFrame nextFrame = allFrames[0];
		for (int i = 1; i < allFrames.size(); i++)
		{
			nextFrame = allFrames[i];
			if (nextFrame.GetTimeStamp() > m_AnimationTime)
				break;
			previousFrame = allFrames[i];
		}

		std::vector<KeyFrame> toReturn;
		toReturn.push_back(previousFrame);
		toReturn.push_back(nextFrame);
		return toReturn;
	}

	float Animator::CalculateProgression(KeyFrame previousFrame, KeyFrame nextFrame)
	{
		float totalTime = nextFrame.GetTimeStamp() - previousFrame.GetTimeStamp();
		float currentTime = m_AnimationTime - previousFrame.GetTimeStamp();
		return currentTime / totalTime;
	}
	std::map<std::string, Matrix> Animator::InterpolatePoses(KeyFrame previousFrame, KeyFrame nextFrame, float progression)
	{
		std::map<std::string, Matrix> currentPose;
		for (std::map<std::string, Transformation>::iterator it = previousFrame.GetPose().begin; it != previousFrame.GetPose().end(); ++it)
		{
			Transformation previousTransform = it->second;
			Transformation nextTransform = nextFrame.GetPose()[it->first];
			Transformation currentTransform = Transformation::Interpolate(previousTransform, nextTransform, progression);
			currentPose[it->first] = currentTransform.ToMatrix();
		}

		return currentPose;
	}
}