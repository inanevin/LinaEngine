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

#include "ECS/ECSComponent.hpp"
#include "Utility/Math/Color.hpp"



namespace LinaEngine::ECS
{
	struct AmbientLightComponent : public ECSComponent<AmbientLightComponent>
	{
		LinaEngine::Color color = LinaEngine::Color(1.0f, 1.0f, 1.0f);
		float intensity = 1.0f;
	};

	struct DiffuseLightComponent : public ECSComponent<DiffuseLightComponent>
	{
		LinaEngine::Color color;
		float intensity;
	};

	struct PointLightComponent : public ECSComponent<PointLightComponent>
	{
		
	};
}


#endif