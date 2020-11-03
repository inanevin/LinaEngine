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
Class: PlayerMotionComponent

Holds information about the player motion, like movement & camera rotation parameters.

Timestamp: 11/3/2020 1:48:06 AM
*/

#pragma once

#ifndef PlayerMotionComponent_HPP
#define PlayerMotionComponent_HPP

#include "ECS/ECSComponent.hpp"
#include "Utility/Math/Vector.hpp"

namespace LinaEngine::ECS
{
	struct PlayerMotionComponent : public ECSComponent
	{
		Vector2 m_movementSpeeds = Vector2::Zero;
		Vector2 m_rotationSpeeds = Vector2::Zero;
		Vector2 m_movementSmooths = Vector2::Zero;
		Vector2 m_rotationSmooths = Vector2::Zero;


		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_movementSpeeds, m_rotationSpeeds, m_movementSmooths, m_rotationSmooths);
		}
	};
}

#endif
