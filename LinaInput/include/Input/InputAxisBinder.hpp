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
Class: InputAxisBinder

Used for defining and abstract idea of "axis".
Basically subscribes various key actions and stores their values into an arbitrary axis structure.

Timestamp: 4/13/2019 10:02:03 PM
*/

#pragma once

#ifndef InputAxisBinder_HPP
#define InputAxisBinder_HPP

#include "Interfaces/IInputSubscriber.hpp"

namespace LinaEngine::Input
{

	class InputMouseButtonBinder : public IInputSubscriber
	{
	public:

		InputMouseButtonBinder() {};
		~InputMouseButtonBinder() {};

		// Upon initialization we immediately listen to target mouse events.
		void Initialize(InputCode::Mouse button = InputCode::Mouse::MouseUnknown)
		{
			SubscribeMousePressedAction("buttonDown", LINA_ACTION_CALLBACK(InputMouseButtonBinder::OnButtonDown), button);
			SubscribeMouseReleasedAction("buttonUp", LINA_ACTION_CALLBACK(InputMouseButtonBinder::OnButtonUp), button);
		}

		void OnButtonDown() { m_isPressed = true; }
		void OnButtonUp() { m_isPressed = false; }
		bool GetIsPressed() { return m_isPressed; }

	private:

		bool m_isPressed = false;
	};

	class InputKeyAxisBinder : public IInputSubscriber
	{
	public:

		InputKeyAxisBinder() {};
		virtual ~InputKeyAxisBinder() {};

		// Upon initialization we immediately listen to target key events.
		void Initialize(InputCode::Key positive, InputCode::Key negative)
		{
			SubscribeKeyPressedAction("posD", LINA_ACTION_CALLBACK(InputKeyAxisBinder::OnPositiveKeyDown), positive);
			SubscribeKeyPressedAction("negD", LINA_ACTION_CALLBACK(InputKeyAxisBinder::OnNegativeKeyDown), negative);
			SubscribeKeyReleasedAction("posU", LINA_ACTION_CALLBACK(InputKeyAxisBinder::OnPositiveKeyUp), positive);
			SubscribeKeyReleasedAction("negU", LINA_ACTION_CALLBACK(InputKeyAxisBinder::OnNegativeKeyUp), negative);
		}

		void OnPositiveKeyDown()
		{
			m_amount = 1.0f;
			m_positivePressed = true;
			if (m_negativePressed)
				OnNegativeKeyUp();
		}

		void OnNegativeKeyDown()
		{
			m_amount = -1.0f;
			m_negativePressed = true;
			if (m_positivePressed)
				OnPositiveKeyUp();
		}

		void OnPositiveKeyUp()
		{
			m_positivePressed = false;
			if (!m_negativePressed)
				m_amount = 0.0f;
		}

		void OnNegativeKeyUp()
		{
			m_negativePressed = false;
			if (!m_positivePressed)
				m_amount = 0.0f;
		}

		float GetAmount() const { return m_amount; }

	private:

		float m_amount = 0.0f;
		bool m_positivePressed = false;
		bool m_negativePressed = false;
	};
}


#endif