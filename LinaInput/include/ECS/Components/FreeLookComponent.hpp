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
Class: FreeLookComponent

Used for free look functionality, much like a fly-camera.

Timestamp: 5/2/2019 1:40:16 AM
*/

#pragma once

#ifndef FreeLookComponent_HPP
#define FreeLookComponent_HPP

#include "ECS/ECSComponent.hpp"
#include "Utility/Math/Vector.hpp"

namespace LinaEngine::ECS
{
	struct FreeLookComponent : public ECSComponent
	{
		LinaEngine::Vector2 m_angles;
		LinaEngine::Vector2 m_movementSpeeds = LinaEngine::Vector2(3, 3);
		LinaEngine::Vector2 m_rotationSpeeds = LinaEngine::Vector2(5, 5);

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_movementSpeeds, m_rotationSpeeds, m_angles, m_isEnabled);
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs.emplace<FreeLookComponent>(entity, FreeLookComponent()); }
#endif
	};
}


#endif