/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: InputEngine
Timestamp: 4/14/2019 7:46:20 PM

*/

#pragma once

#ifndef INPUTDEVICE_HPP
#define INPUTDEVICE_HPP

#include "Lina/Input/InputAxisBinder.hpp"

namespace LinaEngine
{

	template<class Derived>
	class InputEngine
	{
	public:

		virtual ~InputEngine()
		{
			LINA_CORE_TRACE("[Destructor] -> InputDevice ({0})", typeid(*this).name());
		};

		FORCEINLINE void Initialize(void* contextWindowPointer) 
		{
			m_HorizontalAxis.Initialize(Input::Key::L, Input::Key::J);
			m_VerticalAxis.Initialize(Input::Key::I, Input::Key::K);
			m_Derived->Initialize_Impl(contextWindowPointer);
		};

		FORCEINLINE void Tick() { m_Derived->Tick_Impl(); }
		FORCEINLINE bool GetKey(int keyCode) { return m_Derived->GetKey_Impl(keyCode); }
		FORCEINLINE bool GetKeyDown(int keyCode) { return m_Derived->GetKeyDown_Impl(keyCode); }
		FORCEINLINE bool GetKeyUp(int keyCode) { return m_Derived->GetKeyUp_Impl(keyCode); }
		FORCEINLINE bool GetMouseButton(int button) { return m_Derived->GetMouseButton_Impl(button); }
		FORCEINLINE bool GetMouseButtonDown(int button) { return m_Derived->GetMouseButtonDown_Impl(button); }
		FORCEINLINE bool GetMouseButtonUp(int button) { return m_Derived->GetMouseButtonUp_Impl(button); }
		FORCEINLINE Vector2F GetRawMouseAxis() { return m_Derived->GetRawMouseAxis_Impl(); }
		FORCEINLINE Vector2F GetMouseAxis() { return m_Derived->GetMouseAxis_Impl(); }
		FORCEINLINE Vector2F GetMousePosition() { return m_Derived->GetMousePosition_Impl(); }
		FORCEINLINE void SetMousePosition(const Vector2F& v) { m_Derived->SetMousePosition_Impl(v); }
		FORCEINLINE void SetCursor(bool visible) { m_Derived->SetCursor(visible); }

		FORCEINLINE InputKeyAxisBinder& GetHorizontalInput() { return m_HorizontalAxis; };
		FORCEINLINE InputKeyAxisBinder& GetVerticalInput() { return m_VerticalAxis; };
		FORCEINLINE ActionDispatcher& GetInputDispatcher() { return m_InputDispatcher; }

		FORCEINLINE void DispatchKeyAction(Input::Key key, int action)
		{
			if (action == GLFW_PRESS)
			{
				m_InputDispatcher.DispatchAction<Input::Key>(ActionType::KeyPressed, key);
			}
			else if (action == GLFW_RELEASE)
			{
				m_InputDispatcher.DispatchAction<Input::Key>(ActionType::KeyReleased, key);
			}
		}

		FORCEINLINE void DispatchMouseAction(Input::Mouse button, int action)
		{
			if (action == GLFW_PRESS)
			{
				m_InputDispatcher.DispatchAction<Input::Mouse>(ActionType::MouseButtonPressed, button);
			}
			else if (action == GLFW_RELEASE)
			{
				m_InputDispatcher.DispatchAction<Input::Mouse>(ActionType::MouseButtonReleased, button);
			}
		}

	protected:

		FORCEINLINE InputEngine()
		{
			m_Derived = static_cast<Derived*>(this);		
			LINA_CORE_TRACE("[Constructor] -> InputDevice ({0})", typeid(*this).name());
		};

		InputKeyAxisBinder m_HorizontalAxis;
		InputKeyAxisBinder m_VerticalAxis;

	private:

		
		ActionDispatcher m_InputDispatcher;
		Derived* m_Derived;
	};

}


#endif



