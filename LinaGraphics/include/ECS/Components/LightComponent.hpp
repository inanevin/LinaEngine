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
Class: LightComponent

All light types are declared here.

Timestamp: 5/13/2019 9:00:55 PM
*/

#pragma once

#ifndef LightComponent_HPP
#define LightComponent_HPP

#include "Utility/Math/Color.hpp"
#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Math.hpp"
#include "ECS/ECSComponent.hpp"


namespace LinaEngine::ECS
{

	struct LightComponent : public ECSComponent
	{
		Color m_color = Color::White;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_color, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<LightComponent>(entity, LightComponent()); }
#endif
	};

	struct PointLightComponent : public LightComponent
	{
		float m_distance = 0;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_distance, m_color, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<PointLightComponent>(entity, PointLightComponent()); }
#endif
	};

	struct SpotLightComponent : public LightComponent
	{
		float m_distance = 0;
		float m_cutoff = Math::Cos(Math::ToRadians(12.5f));
		float m_outerCutoff = Math::Cos(Math::ToRadians(17.5f));

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_color, m_distance, m_cutoff, m_outerCutoff, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<SpotLightComponent>(entity, SpotLightComponent()); }
#endif
	};

	struct DirectionalLightComponent : public LightComponent
	{
		Vector4 m_shadowOrthoProjection = Vector4(-20, 20, -20, 20);
		float m_shadowZNear = 10.0f;
		float m_shadowZFar = 15.0f;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(m_shadowOrthoProjection, m_shadowZNear, m_shadowZFar, m_color, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<DirectionalLightComponent>(entity, DirectionalLightComponent()); }
#endif
	};
}


#endif