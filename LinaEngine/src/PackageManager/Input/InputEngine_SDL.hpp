/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: InputEngine_SDL
Timestamp: 1/6/2019 2:18:10 AM

*/

#pragma once
#ifndef InputEngine_SDL_HPP
#define InputEngine_SDL_HPP

#include "Lina/Input/InputEngine.hpp"

namespace LinaEngine
{
	
	class LINA_API InputEngine_SDL : public InputEngine
	{
	public:

		InputEngine_SDL();
		~InputEngine_SDL();

		void OnUpdate() override;
		void OnInputEvent(Event& e) override;


	};
}


#endif