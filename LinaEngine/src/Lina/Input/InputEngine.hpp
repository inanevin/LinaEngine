/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: InputEngine
Timestamp: 1/6/2019 2:17:55 AM

*/

#pragma once
#ifndef InputEngine_HPP
#define InputEngine_HPP

#include "InputMappings.hpp"

namespace LinaEngine
{


	class InputEngine
	{

	public:

		InputEngine();
		virtual ~InputEngine();

		virtual void OnUpdate() = 0;
		virtual bool GetKey(int keyCode) = 0;
		virtual bool GetKeyDown(int keyCode) = 0;
		virtual bool GetKeyUp(int keyCode) = 0;
		virtual bool GetMouse(int keyCode) = 0;
		virtual bool GetMouseDown(int mouse) = 0;
		virtual bool GetMouseUp(int mouse) = 0;
		virtual Vector2F GetRawMouse() = 0;
		virtual Vector2F GetMouse() = 0;

	};
}


#endif