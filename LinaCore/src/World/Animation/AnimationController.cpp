/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Core/World/Animation/AnimationController.hpp"
#include "Core/World/Components/CompModel.hpp"

namespace Lina
{
	void AnimationController::TickAnimation(float delta)
	{
		if (m_animationIndex == -1)
			return;

		Model*						  model		 = m_comp->GetModelPtr();
		const Vector<ModelAnimation>& anims		 = model->GetAllAnimations();
		const ModelAnimation&		  targetAnim = anims.at(m_animationIndex);

		Vector<CompModelNode>& nodes = m_comp->GetNodes();

		for (const ModelAnimationChannelV3& ch : targetAnim.positionChannels)
		{
			const Vector3  position = ch.Sample(m_animationTime);
			CompModelNode& node		= nodes.at(ch.nodeIndex);
			node.transform.SetLocalPosition(position);
		}

		for (const ModelAnimationChannelV3& ch : targetAnim.scaleChannels)
		{
			const Vector3  scale = ch.Sample(m_animationTime);
			CompModelNode& node	 = nodes.at(ch.nodeIndex);
			node.transform.SetLocalScale(scale);
		}

		for (const ModelAnimationChannelQ& ch : targetAnim.rotationChannels)
		{
			const Quaternion rotation = ch.Sample(m_animationTime);
			CompModelNode&	 node	  = nodes.at(ch.nodeIndex);
			node.transform.SetLocalRotation(rotation);
		}

		m_animationTime += delta * m_speed;
		if (m_animationTime > targetAnim.duration)
			m_animationTime = 0.0f;
	}
} // namespace Lina
