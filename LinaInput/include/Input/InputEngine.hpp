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

#include "Input/InputAxisBinder.hpp"
#include "Utility/Math/Vector.hpp"
#include "Input/InputCommon.hpp"
#include "InputDevice.hpp"

namespace LinaEngine::Input
{
	class InputEngine : public LinaEngine::Action::ActionDispatcher
	{

	public:


		FORCEINLINE InputEngine() : LinaEngine::Action::ActionDispatcher()
		{
			LINA_CORE_TRACE("[Constructor] -> InputEngine ({0})", typeid(*this).name());
		};

		virtual ~InputEngine()
		{
			LINA_CORE_TRACE("[Destructor] -> InputEngine ({0})", typeid(*this).name());
		};
		
		// Initialize the engine, sets the dispatcher references & initializes axes.
		FORCEINLINE void Initialize(void* contextWindowPointer, InputDevice* inputDevice) 
		{
			m_inputDevice = inputDevice;
			m_horizontalKeyAxis.SetActionDispatcher(this);
			m_verticalKeyAxis.SetActionDispatcher(this);
			m_horizontalKeyAxis.Initialize(InputCode::Key::D, InputCode::Key::A);
			m_verticalKeyAxis.Initialize(InputCode::Key::W, InputCode::Key::S);
			m_inputDevice->Initialize(contextWindowPointer);
		};

		FORCEINLINE void Tick() { m_inputDevice->Tick(); }

		FORCEINLINE bool GetKey(int keyCode) { return m_inputDevice->GetKey(keyCode); }

		FORCEINLINE bool GetKeyDown(int keyCode) { return m_inputDevice->GetKeyDown(keyCode); }

		FORCEINLINE bool GetKeyUp(int keyCode) { return m_inputDevice->GetKeyUp(keyCode); }

		FORCEINLINE bool GetMouseButton(int button) { return m_inputDevice->GetMouseButton(button); }

		FORCEINLINE bool GetMouseButtonDown(int button) { return m_inputDevice->GetMouseButtonDown(button); }

		FORCEINLINE bool GetMouseButtonUp(int button) { return m_inputDevice->GetMouseButtonUp(button); }

		FORCEINLINE Vector2 GetRawMouseAxis() { return m_inputDevice->GetRawMouseAxis(); }

		FORCEINLINE Vector2 GetMouseAxis() { return m_inputDevice->GetMouseAxis(); }

		FORCEINLINE Vector2 GetMousePosition() { return m_inputDevice->GetMousePosition(); }

		FORCEINLINE void SetMousePosition(const Vector2& v) { m_inputDevice->SetMousePosition(v); }

		FORCEINLINE void SetCursorMode(CursorMode cursorMode) { m_inputDevice->SetCursorMode(cursorMode); }

		FORCEINLINE float GetHorizontalAxisValue() { return m_horizontalKeyAxis.GetAmount(); }

		FORCEINLINE float GetVerticalAxisValue() { return m_verticalKeyAxis.GetAmount(); }

		FORCEINLINE void DispatchKeyAction(InputCode::Key key, int action)
		{
			if (action == 1)
				DispatchAction<InputCode::Key>(LinaEngine::Action::ActionType::KeyPressed, key);
			else if (action == 0)
				DispatchAction<InputCode::Key>(LinaEngine::Action::ActionType::KeyReleased, key);
		}

		FORCEINLINE void DispatchMouseAction(InputCode::Mouse button, int action)
		{
			if (action == 1)
				DispatchAction<InputCode::Mouse>(LinaEngine::Action::ActionType::MouseButtonPressed, button);
			else if (action == 0)
				DispatchAction<InputCode::Mouse>(LinaEngine::Action::ActionType::MouseButtonReleased, button);
		}


	private:

		// Axis binders.
		InputKeyAxisBinder m_horizontalKeyAxis;
		InputKeyAxisBinder m_verticalKeyAxis;
		InputDevice* m_inputDevice;

	};

}


#endif



