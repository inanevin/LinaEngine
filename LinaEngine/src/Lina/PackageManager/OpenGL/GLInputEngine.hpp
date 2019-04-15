/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GLInputEngine
Timestamp: 4/14/2019 5:15:15 PM

*/

#pragma once

#ifndef GLInputDevice_HPP
#define GLInputDevice_HPP


#include "Lina/Input/InputEngine.hpp"


namespace LinaEngine
{
	// A subclass of Input Engine. Check the InputEngine.hpp for method details.
	class GLInputEngine : public InputEngine<GLInputEngine>
	{
	public:

		GLInputEngine();
		virtual ~GLInputEngine();

		void Initialize_Impl(void* contextWindowPointer);
		void Tick_Impl();
		bool GetKey_Impl(int keyCode);
		bool GetKeyDown_Impl(int keyCode);
		bool GetKeyUp_Impl(int keyCode);
		bool GetMouseButton_Impl(int index);
		bool GetMouseButtonDown_Impl(int index);
		bool GetMouseButtonUp_Impl(int index);
		Vector2F GetRawMouseAxis_Impl();
		Vector2F GetMouseAxis_Impl();
		Vector2F GetMousePosition_Impl();
		void SetCursor_Impl(bool visible) const;
		void SetMousePosition_Impl(const Vector2F& v) const;

	private:

		int previousKeys[NUM_KEY_STATES];
		int currentKeys[NUM_KEY_STATES];
	};
}


#endif