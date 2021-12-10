/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: FreeLookSystem

System that manages free look components and the functionality.

Timestamp: 5/2/2019 2:21:35 AM
*/

#pragma once

#ifndef FreeLookSystem_HPP
#define FreeLookSystem_HPP

#include "ECS/ECS.hpp"

namespace Lina::Input
{
	class InputEngine;
}

namespace Lina::ECS
{
	class FreeLookSystem : public BaseECSSystem
	{
	public:

		void Construct(Registry& registry, Lina::Input::InputEngine& inputEngineIn)
		{
			BaseECSSystem::Construct(registry);
			m_inputEngine = &inputEngineIn;
		}

		virtual void UpdateComponents(float delta) override;

	private:

		Lina::Input::InputEngine* m_inputEngine;
		float m_horizontalKeyAmt = 0.0f;
		float m_verticalKeyAmt = 0.0f;
		float m_targetXAngle = 0.0f;
		float m_targetYAngle = 0.0f;
	};
}


#endif