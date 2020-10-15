/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: LightComponent
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
		Color color;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(color, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<LightComponent>(entity, LightComponent()); }
#endif
	};

	struct PointLightComponent : public LightComponent
	{
		float distance;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(distance, color, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<PointLightComponent>(entity, PointLightComponent()); }
#endif
	};

	struct SpotLightComponent : public LightComponent
	{
		float distance;
		float cutOff = Math::Cos(Math::ToRadians(12.5f));
		float outerCutOff = Math::Cos(Math::ToRadians(17.5f));

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(color, distance, cutOff, outerCutOff, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<SpotLightComponent>(entity, SpotLightComponent()); }
#endif
	};

	struct DirectionalLightComponent : public LightComponent
	{
		Vector4 shadowProjectionSettings = Vector4(-20, 20, -20, 20);
		float shadowNearPlane = 10.0f;
		float shadowFarPlane = 15.0f;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(shadowProjectionSettings, shadowNearPlane, shadowFarPlane, color, m_isEnabled); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG;
		COMPONENT_ADDFUNC_SIG{ ecs->emplace<DirectionalLightComponent>(entity, DirectionalLightComponent()); }
#endif
	};
}


#endif