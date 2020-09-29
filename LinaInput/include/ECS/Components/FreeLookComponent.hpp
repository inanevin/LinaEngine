/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: FreeLookComponent
Timestamp: 5/2/2019 1:40:16 AM

*/

#pragma once

#ifndef FreeLookComponent_HPP
#define FreeLookComponent_HPP

#include "ECS/ECSComponent.hpp"

namespace LinaEngine::ECS
{
	struct FreeLookComponent : public ECSComponent
	{
		float movementSpeedX = 0.0f;
		float movementSpeedZ = 0.0f;
		float rotationSpeedX = 0.0f;
		float rotationSpeedY = 0.0f;
		float horizontalAngle = 0.0f;
		float verticalAngle = 0.0f;


		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(movementSpeedX, movementSpeedZ, rotationSpeedX, rotationSpeedY, horizontalAngle, verticalAngle); // serialize things by passing them to the archive
		}
	};
}


#endif