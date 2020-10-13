/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: CameraComponent
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
		LinaEngine::Color clearColor = LinaEngine::Color(0.1f, 0.1f, 0.1f, 1.0f);
		float fieldOfView = 90.0f;
		float zNear = 0.01f;
		float zFar = 1000.0f;
		bool isActive = false;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(clearColor, fieldOfView, zNear, zFar, isActive); // serialize things by passing them to the archive
		}

#ifdef LINA_EDITOR
		COMPONENT_DRAWFUNC_SIG
#endif

	};
}


#endif