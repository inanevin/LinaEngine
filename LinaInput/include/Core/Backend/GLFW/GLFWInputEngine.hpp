/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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
Class: InputEngine

Acts as a wrapper for input device functionality and also manages any additional functionality.

Timestamp: 4/14/2019 7:46:20 PM
*/

#pragma once

#ifndef GLFWInputEngine_HPP
#define GLFWInputEngine_HPP

#include "Core/InputCommon.hpp"
#include "Core/InputAxis.hpp"
#include "Core/InputMappings.hpp"
#include "ECS/ECS.hpp"
#include "EventSystem/Events.hpp"
#include "Math/Vector.hpp"

namespace Lina
{
	class Application;
}

namespace Lina::Input
{
	class GLFWInputEngine 
	{

	public:

		static GLFWInputEngine* Get() { return s_inputEngine; }

		bool GetKey(int keyCode);
		bool GetKeyDown(int keyCode);
		bool GetKeyUp(int keyCode);
		bool GetMouseButton(int index);
		bool GetMouseButtonDown(int index);
		bool GetMouseButtonUp(int index);
		void SetCursorMode(CursorMode mode) const;
		void SetMousePosition(const Vector2& v) const;
		void Tick();
		Vector2 GetMousePosition();
		Vector2 GetRawMouseAxis();
		Vector2 GetMouseAxis();
		float GetHorizontalAxisValue() { return m_horizontalAxis.GetValue(); }
		float GetVerticalAxisValue() { return m_verticalAxis.GetValue(); }

	private:

		friend class Application;

		GLFWInputEngine() { }
		~GLFWInputEngine();

		void Initialize();
		void OnPlayModeChanged(Event::EPlayModeChanged playMode);
		void OnPreMainLoop(Event::EPreMainLoop& e);
		void OnPostMainLoop(Event::EPostMainLoop& e);
		void OnWindowContextCreated(Event::EWindowContextCreated& e);

	private:

		friend class Lina::Application;
		static GLFWInputEngine* s_inputEngine;
		InputAxis m_horizontalAxis;
		InputAxis m_verticalAxis;
			
	};
}

#endif



