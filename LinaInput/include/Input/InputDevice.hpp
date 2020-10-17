/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
Class: InputDevice

Responsible for interacting with the hardware, abstract class.

Timestamp: 10/6/2020 11:13:50 AM
*/

#pragma once

#ifndef InputDevice_HPP
#define InputDevice_HPP



#include "Utility/Math/Vector.hpp"
#include "Input/InputMappings.hpp"
#include "Input/InputCommon.hpp"

namespace LinaEngine::Input
{
	class InputDevice
	{

	public:

		InputDevice() {};
		~InputDevice() {};

		virtual void Initialize(void* contextWindowPointer) = 0;
		virtual void Tick() = 0;
		virtual bool GetKey(int keyCode) = 0;
		virtual bool GetKeyDown(int keyCode) = 0;
		virtual bool GetKeyUp(int keyCode) = 0;
		virtual bool GetMouseButton(int index) = 0;
		virtual bool GetMouseButtonDown(int index) = 0;
		virtual bool GetMouseButtonUp(int index) = 0;
		virtual Vector2 GetMousePosition() = 0;
		virtual void SetCursorMode(CursorMode mode) const = 0;
		virtual void SetMousePosition(const Vector2& v) const = 0;

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.
		virtual Vector2 GetRawMouseAxis() = 0;

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.
		virtual Vector2 GetMouseAxis() = 0;

	};
}

#endif
