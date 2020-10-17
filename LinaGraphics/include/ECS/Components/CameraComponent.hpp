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
Class: CameraComponent

Represents a virtual game camera.

Timestamp: 5/2/2019 12:20:42 AM
*/

#pragma once

#ifndef CameraComponent_HPP
#define CameraComponent_HPP

#include "ECS/ECSComponent.hpp"
#include "Utility/Math/Color.hpp"

namespace LinaEngine::ECS
{

	struct CameraComponent : public ECSComponent
	{
		LinaEngine::Color m_clearColor = LinaEngine::Color(0.1f, 0.1f, 0.1f, 1.0f);
		float m_fieldOfView = 90.0f;
		float m_zNear = 0.01f;
		float m_zFar = 1000.0f;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_clearColor, m_fieldOfView, m_zNear, m_zFar, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<CameraComponent>(entity, CameraComponent()); }
#endif

	};
}


#endif