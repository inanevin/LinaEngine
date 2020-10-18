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
Class: InputEngine

Acts as a wrapper for input device functionality and also manages any additional functionality.

Timestamp: 4/14/2019 7:46:20 PM
*/

#pragma once

#ifndef INPUTENGINE_HPP
#define INPUTENGINE_HPP

#include "Utility/Math/Vector.hpp"
#include "Input/InputCommon.hpp"
#include "Actions/ActionDispatcher.hpp"
#include "InputDevice.hpp"
#include "InputAxisBinder.hpp"
namespace LinaEngine::Input
{
	class InputEngine
	{

	public:

		InputEngine() {};
		virtual ~InputEngine() {};

		// Initialize the engine, sets the dispatcher references & initializes axes.
		void Initialize(void* contextWindowPointer, InputDevice* inputDevice);

		void Tick() { m_inputDevice->Tick(); }

		bool GetKey(int keyCode) { return m_inputDevice->GetKey(keyCode); }

		bool GetKeyDown(int keyCode) { return m_inputDevice->GetKeyDown(keyCode); }

		bool GetKeyUp(int keyCode) { return m_inputDevice->GetKeyUp(keyCode); }

		bool GetMouseButton(int button) { return m_inputDevice->GetMouseButton(button); }

		bool GetMouseButtonDown(int button) { return m_inputDevice->GetMouseButtonDown(button); }

		bool GetMouseButtonUp(int button) { return m_inputDevice->GetMouseButtonUp(button); }

		Vector2 GetRawMouseAxis() { return m_inputDevice->GetRawMouseAxis(); }

		Vector2 GetMouseAxis() { return m_inputDevice->GetMouseAxis(); }

		Vector2 GetMousePosition() { return m_inputDevice->GetMousePosition(); }

		void SetMousePosition(const Vector2& v) { m_inputDevice->SetMousePosition(v); }

		void SetCursorMode(CursorMode cursorMode) { m_inputDevice->SetCursorMode(cursorMode); }

		float GetHorizontalAxisValue() { return m_horizontalKeyAxis.GetAmount(); }

		float GetVerticalAxisValue() { return m_verticalKeyAxis.GetAmount(); }

		void DispatchKeyAction(InputCode::Key key, int action)
		{
			if (action == 1)
				s_inputDispatcher.DispatchAction<InputCode::Key>(LinaEngine::Action::ActionType::KeyPressed, key);
			else if (action == 0)
				s_inputDispatcher.DispatchAction<InputCode::Key>(LinaEngine::Action::ActionType::KeyReleased, key);
		}

		void DispatchMouseAction(InputCode::Mouse button, int action)
		{
			if (action == 1)
				s_inputDispatcher.DispatchAction<InputCode::Mouse>(LinaEngine::Action::ActionType::MouseButtonPressed, button);
			else if (action == 0)
				s_inputDispatcher.DispatchAction<InputCode::Mouse>(LinaEngine::Action::ActionType::MouseButtonReleased, button);
		}

		static Action::ActionDispatcher& GetInputDispatcher() { return s_inputDispatcher; }

	private:

		// Axis binders.
		InputKeyAxisBinder m_horizontalKeyAxis;
		InputKeyAxisBinder m_verticalKeyAxis;

		static Action::ActionDispatcher s_inputDispatcher;
		InputDevice* m_inputDevice = nullptr;

		DISALLOW_COPY_ASSIGN_MOVE(InputEngine)


	};
}


#endif



