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

Class: ComponentDrawer
Timestamp: 10/13/2020 2:34:21 PM

*/
#pragma once

#ifndef ComponentDrawer_HPP
#define ComponentDrawer_HPP

// Headers here.
#include "ECS/ECS.hpp"
#include <functional>
#include <map>

namespace LinaEditor
{
	class ComponentDrawer
	{
		
	public:
		
		static void RegisterComponentFunctions();

	public:

		static std::map<entt::id_type, std::function<void(LinaEngine::ECS::ECSEntity)>> s_componentDrawFuncMap;
		static std::vector<std::string> GetEligibleComponents(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity);
		static void AddComponentToEntity(LinaEngine::ECS::ECSRegistry* ecs, LinaEngine::ECS::ECSEntity entity, const std::string& comp);
	
	};
}

#endif
