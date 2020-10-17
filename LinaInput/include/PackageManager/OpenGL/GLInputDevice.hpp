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
Class: GLInputDevice

OpenGL implementation of an input device using GLFW for hardware interaction.

Timestamp: 4/14/2019 5:15:15 PM
*/

#pragma once

#ifndef GLInputDevice_HPP
#define GLInputDevice_HPP

#include "Input/InputDevice.hpp"

namespace LinaEngine::Input
{
	// A subclass of Input Device.
	class GLInputDevice : public InputDevice
	{

	public:

		GLInputDevice();
		virtual ~GLInputDevice();

		void Initialize(void* contextWindowPointer) override;
		void Tick() override;
		bool GetKey(int keyCode) override;
		bool GetKeyDown(int keyCode) override;
		bool GetKeyUp(int keyCode) override;
		bool GetMouseButton(int index) override;
		bool GetMouseButtonDown(int index) override;
		bool GetMouseButtonUp(int index) override;
		Vector2 GetMousePosition() override;
		void SetCursorMode(CursorMode mode) const override;
		void SetMousePosition(const Vector2& v) const override;

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.
		Vector2 GetRawMouseAxis() override;

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.
		Vector2 GetMouseAxis() override;

	private:

		int m_previousKeys[NUM_KEY_STATES];
		int m_currentKeys[NUM_KEY_STATES];
	};
}

#endif