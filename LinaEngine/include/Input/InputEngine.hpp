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

#include "Input/InputAxisBinder.hpp"
#include "Utility/Math/Vector.hpp"
namespace LinaEngine::Input
{
	// Templated base class for various input engines. Defines most operations as inlined methods that call subclass methods.
	template<class Derived>
	class InputEngine : public ActionDispatcher
	{
	public:

		virtual ~InputEngine()
		{
			LINA_CORE_TRACE("[Destructor] -> InputEngine ({0})", typeid(*this).name());
		};
		
		// Initialize the engine, sets the dispatcher references & initializes axes.
		FORCEINLINE void Initialize(void* contextWindowPointer) 
		{
			// Assign unique ptrs
			m_HorizontalKeyAxis = std::make_unique<InputKeyAxisBinder>();
			m_VerticalKeyAxis = std::make_unique<InputKeyAxisBinder>();

			// Set the action dispatchers as our dispatcher.
			m_HorizontalKeyAxis->SetActionDispatcher(this);
			m_VerticalKeyAxis->SetActionDispatcher(this);

			// Initialize the axes.
			m_HorizontalKeyAxis->Initialize(InputCode::Key::D, InputCode::Key::A);
			m_VerticalKeyAxis->Initialize(InputCode::Key::W, InputCode::Key::S);

			// Initialize subclass.
			m_Derived->Initialize_Impl(contextWindowPointer);
		};

		// Called each frame.
		FORCEINLINE void Tick() { m_Derived->Tick_Impl(); }

		// Returns true each frame key mapped with the keyCode is pressed
		FORCEINLINE bool GetKey(int keyCode) { return m_Derived->GetKey_Impl(keyCode); }

		// Returns true in the frame key mapped with the keyCode is pressed.
		FORCEINLINE bool GetKeyDown(int keyCode) { return m_Derived->GetKeyDown_Impl(keyCode); }

		// Returns true in the frame key mapped with the keyCode is stopped being pressed.
		FORCEINLINE bool GetKeyUp(int keyCode) { return m_Derived->GetKeyUp_Impl(keyCode); }

		// Returns true each frame mouse button mapped with the index is pressed
		FORCEINLINE bool GetMouseButton(int button) { return m_Derived->GetMouseButton_Impl(button); }

		// Returns true in the frame mouse button mapped with the index is pressed. 
		FORCEINLINE bool GetMouseButtonDown(int button) { return m_Derived->GetMouseButtonDown_Impl(button); }

		// Returns true in the frame mouse mapped with the index is stopped being pressed.
		FORCEINLINE bool GetMouseButtonUp(int button) { return m_Derived->GetMouseButtonUp_Impl(button); }

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Not smoothed.
		FORCEINLINE Vector2F GetRawMouseAxis() { return m_Derived->GetRawMouseAxis_Impl(); }

		// Returns a Vector2 with parameters ranging from -1 to 1 for X & Y. Delta smoothed.
		FORCEINLINE Vector2F GetMouseAxis() { return m_Derived->GetMouseAxis_Impl(); }

		// Returns a Vector2 containing screen space mouse positions
		FORCEINLINE Vector2F GetMousePosition() { return m_Derived->GetMousePosition_Impl(); }

		// Returns the current value of the horizontal key axis.
		FORCEINLINE float GetHorizontalAxisValue() { return m_HorizontalKeyAxis->GetAmount(); }

		// Returns the current value of the vertical key axis.
		FORCEINLINE float GetVerticalAxisValue() { return m_VerticalKeyAxis->GetAmount(); }

		// Returns the horizontal key axis binder.
		FORCEINLINE InputKeyAxisBinder* GetHorizontalKeyAxis() { return m_HorizontalKeyAxis.get(); }

		// Returns the vertical key axis binder.
		FORCEINLINE InputKeyAxisBinder* GetVerticalKeyAxis() { return m_VerticalKeyAxis.get(); }

		// Set mouse position.
		FORCEINLINE void SetMousePosition(const Vector2F& v) { m_Derived->SetMousePosition_Impl(v); }

		// Set cursor visible/invisible.
		FORCEINLINE void SetCursor(bool visible) { m_Derived->SetCursor_Impl(visible); }

		// Get the input dispatcher of this engine.
		FORCEINLINE ActionDispatcher& GetInputDispatcher() { return m_InputDispatcher; }

		// Dispatches a key action to listeners.
		FORCEINLINE void DispatchKeyAction(InputCode::Key key, int action)
		{
			if (action == GLFW_PRESS)
			{
				DispatchAction<InputCode::Key>(ActionType::KeyPressed, key);
			}
			else if (action == GLFW_RELEASE)
			{
				DispatchAction<InputCode::Key>(ActionType::KeyReleased, key);
			}
		}

		// Dispatches a mouse action to listeners.
		FORCEINLINE void DispatchMouseAction(InputCode::Mouse button, int action)
		{
			if (action == GLFW_PRESS)
			{
				DispatchAction<InputCode::Mouse>(ActionType::MouseButtonPressed, button);
			}
			else if (action == GLFW_RELEASE)
			{
				DispatchAction<InputCode::Mouse>(ActionType::MouseButtonReleased, button);
			}
		}

	protected:

		FORCEINLINE InputEngine() : ActionDispatcher()
		{
			m_Derived = static_cast<Derived*>(this);		
			LINA_CORE_TRACE("[Constructor] -> InputEngine ({0})", typeid(*this).name());
		};

	private:

		// Axis binders.
		std::unique_ptr<InputKeyAxisBinder> m_HorizontalKeyAxis;
		std::unique_ptr<InputKeyAxisBinder> m_VerticalKeyAxis;

		// Derived class reference for static polymorphism.
		Derived* m_Derived;

	};

}


#endif



