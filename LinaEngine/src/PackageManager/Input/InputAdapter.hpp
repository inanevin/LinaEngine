/*
Author: Inan Evin
www.inanevin.com
Copyright 2018 Inan Evin
Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.
Class: InputAdapter
Timestamp: 1/6/2019 2:16:29 AM
*/

#pragma once
#ifndef InputAdapter_HPP
#define InputAdapter_HPP


#ifdef LLF_INPUTANDWINDOW_SDL

#include "InputEngine_SDL.hpp";

#define INPUTENGINE_CREATEFUNC(PARAM) inline InputEngine* CreateInputEngine() { return new InputEngine_SDL(); }

#elif LLF_INPUTANDWINDOW_GLFW

#include "InputEngine_GLFW.hpp"
#define INPUTENGINE_CREATEFUNC(PARAM) inline InputEngine* CreateInputEngine() { return new InputEngine_GLFW(); }

#else

#include "Lina/Input/InputEngine.hpp"

#define INPUTENGINE_CREATEFUNC(PARAM) inline InputEngine* CreateInputEngine() { LINA_CORE_ERR("No LLF is defined for input! Aborting!");  exit(EXIT_FAILURE); };

#endif

namespace LinaEngine
{
	class InputAdapter
	{
	public:

		InputAdapter() {};
		~InputAdapter() {};
		INPUTENGINE_CREATEFUNC(PARAM);

	};
}


#endif