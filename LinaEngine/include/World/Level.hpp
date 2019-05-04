/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Level
Timestamp: 5/4/2019 3:33:24 PM

*/

#pragma once

#ifndef Level_HPP
#define Level_HPP

#include "Core/APIExport.hpp"

namespace LinaEngine::World
{
	class Level
	{
	public:

		LINA_API Level() {};
		LINA_API virtual ~Level() {};

		LINA_API virtual void OnLevelLoaded() {};
		LINA_API virtual void OnLevelRemoved() {};
		LINA_API virtual void Install() {};
		LINA_API virtual void Initialize() {};
		LINA_API virtual void Tick(float delta) {};

	};
}


#endif