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

#ifndef INPUTENGINE_HPP
#define INPUTENGINE_HPP

#include "PackageManager/PAMInputDevice.hpp"
#include "Input/InputAxisBinder.hpp"
#include "Utility/Math/Vector.hpp"
#include "Input/InputCommon.hpp"


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
		FORCEINLINE void Initialize(void* contextWindowPointer) 
		{
			// Set the action dispatchers as our dispatcher.
			m_HorizontalKeyAxis.SetActionDispatcher(this);
			m_VerticalKeyAxis.SetActionDispatcher(this);

			// Initialize the axes.
			m_HorizontalKeyAxis.Initialize(InputCode::Key::D, InputCode::Key::A);
			m_VerticalKeyAxis.Initialize(InputCode::Key::W, InputCode::Key::S);

			// Initialize subclass.
			m_InputDevice.Initialize(contextWindowPointer);
		};

		// Called each frame.
		FORCEINLINE void Tick() { m_InputDevice.Tick(); }

		// Returns true each frame key mapped with the keyCode is pressed
		FORCEINLINE bool GetKey(int keyCode) { return m_InputDevice.GetKey(keyCode); }

		// Returns true in the frame key mapped with the keyCode is pressed.
		FORCEINLINE bool GetKeyDown(int keyCode) { return m_InputDevice.GetKeyDown(keyCode); }

		// Returns true in the frame key mapped with the keyCode is stopped being pressed.
		FORCEINLINE bool GetKeyUp(int keyCode) { return m_InputDevice.GetKeyUp(keyCode); }

		// Returns true each frame mouse button mapped with the index is pressed
		FORCEINLINE bool GetMouseButton(int button) { return m_InputDevice.GetMouseButton(button); }

		// Returns true in the frame mouse button mapped with the index is pressed. 
		FORCEINLINE bool GetMouseButtonDown(int button) { return m_InputDevice.GetMouseButtonDown(button); }

		// Returns true in the frame mouse mapped with the index is stopped being pressed.
		FORCEINLINE bool GetMouseButtonUp(int button) { return m_InputDevice.GetMouseButtonUp(button); }

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.
		FORCEINLINE Vector2 GetRawMouseAxis() { return m_InputDevice.GetRawMouseAxis(); }

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.
		FORCEINLINE Vector2 GetMouseAxis() { return m_InputDevice.GetMouseAxis(); }

		// Returns a Vector2 containing screen space mouse positions
		FORCEINLINE Vector2 GetMousePosition() { return m_InputDevice.GetMousePosition(); }

		// Set mouse position.
		FORCEINLINE void SetMousePosition(const Vector2& v) { m_InputDevice.SetMousePosition(v); }

		// Set cursor visible/invisible.
		FORCEINLINE void SetCursorMode(CursorMode cursorMode) { m_InputDevice.SetCursorMode(cursorMode); }

		// Returns the current value of the horizontal key axis.
		FORCEINLINE float GetHorizontalAxisValue() { return m_HorizontalKeyAxis.GetAmount(); }

		// Returns the current value of the vertical key axis.
		FORCEINLINE float GetVerticalAxisValue() { return m_VerticalKeyAxis.GetAmount(); }

		// Dispatches a key action to listeners.
		FORCEINLINE void DispatchKeyAction(InputCode::Key key, int action)
		{
			if (action == 1)
			{
				DispatchAction<InputCode::Key>(LinaEngine::Action::ActionType::KeyPressed, key);
			}
			else if (action == 0)
			{
				DispatchAction<InputCode::Key>(LinaEngine::Action::ActionType::KeyReleased, key);
			}
		}

		// Dispatches a mouse action to listeners.
		FORCEINLINE void DispatchMouseAction(InputCode::Mouse button, int action)
		{
			if (action == 1)
			{
				DispatchAction<InputCode::Mouse>(LinaEngine::Action::ActionType::MouseButtonPressed, button);
			}
			else if (action == 0)
			{
				DispatchAction<InputCode::Mouse>(LinaEngine::Action::ActionType::MouseButtonReleased, button);
			}
		}


	private:

		// Axis binders.
		InputKeyAxisBinder m_HorizontalKeyAxis;
		InputKeyAxisBinder m_VerticalKeyAxis;
		InputDevice m_InputDevice;

	};

}


#endif



