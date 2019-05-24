/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: CubeChunkComponent
Timestamp: 4/30/2019 12:02:18 AM

*/

#pragma once

#ifndef CubeChunkComponent_HPP
#define CubeChunkComponent_HPP

#include "ECS/ECSComponent.hpp"

namespace LinaEngine::ECS
{
	struct CubeChunkComponent : public ECSComponent<CubeChunkComponent>
	{
		float position[3];
		float velocity[3];
		float acceleration[3];
		uint8 textureIndex = 0;
	};
}


#endif