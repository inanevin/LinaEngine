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

Class: ECSComponent
Timestamp: 5/23/2020 5:17:02 PM

*/
#pragma once

#ifndef ECSComponent_HPP
#define ECSComponent_HPP

// Headers here.

#ifdef LINA_EDITOR
#include "ECS/ECS.hpp"
#define COMPONENT_DRAWFUNC_SIG static void Draw(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
#define COMPONENT_DRAWFUNC Draw
#define COMPONENT_ADDFUNC_SIG static void Add(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity)
#define COMPONENT_ADDFUNC Add
#endif

namespace LinaEngine::ECS
{

	class ECSComponent
	{
	public:

		ECSComponent() {};
		virtual ~ECSComponent() {};
		bool m_isEnabled = true;
	
	};
}

#endif
