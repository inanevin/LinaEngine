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

Class: GroundCubeComponent
Timestamp: 6/4/2020 1:43:44 AM

*/
#pragma once

#ifndef GroundCubeComponent_HPP
#define GroundCubeComponent_HPP

#include "ECS/ECSComponent.hpp"

namespace LinaEngine::ECS
{
	struct GroundCubeComponent : public ECSComponent
	{
		Vector3 initialPos;
		Vector3 targetPos;
		float delay;
		float time = 0.0f;

	};
}

#endif
