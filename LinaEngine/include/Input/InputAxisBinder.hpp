/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: InputAxisBinder
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

		FORCEINLINE void Initialize(InputCode::Mouse button = InputCode::Mouse::MouseUnknown)
		{
			IInputSubscriber::Initialize();
			SubscribeMousePressedAction("buttonDown", LINA_ACTION_CALLBACK(InputMouseButtonBinder::OnButtonDown), button);
			SubscribeMouseReleasedAction("buttonUp", LINA_ACTION_CALLBACK(InputMouseButtonBinder::OnButtonUp), button);
		}

		FORCEINLINE void OnButtonDown() { isPressed = true; }
		FORCEINLINE void OnButtonUp() { isPressed = false; }
		FORCEINLINE bool GetIsPressed() { return isPressed; }

	private:

		bool isPressed = false;
	};

	class InputKeyAxisBinder : public IInputSubscriber
	{
	public:

		InputKeyAxisBinder() {};

		~InputKeyAxisBinder() {};

		FORCEINLINE void Initialize(InputCode::Key positive, InputCode::Key negative)
		{
			IInputSubscriber::Initialize();

			SubscribeKeyPressedAction("posD", LINA_ACTION_CALLBACK(InputKeyAxisBinder::OnPositiveKeyDown), positive);
			SubscribeKeyPressedAction("negD", LINA_ACTION_CALLBACK(InputKeyAxisBinder::OnNegativeKeyDown), negative);
			SubscribeKeyReleasedAction("posU", LINA_ACTION_CALLBACK(InputKeyAxisBinder::OnPositiveKeyUp), positive);
			SubscribeKeyReleasedAction("negU", LINA_ACTION_CALLBACK(InputKeyAxisBinder::OnNegativeKeyUp), negative);


		}

		FORCEINLINE void OnPositiveKeyDown() { m_Amount = 1.0f; positivePressed = true; if (negativePressed) OnNegativeKeyUp(); }
		FORCEINLINE void OnNegativeKeyDown() { m_Amount = -1.0f; negativePressed = true; if (positivePressed) OnPositiveKeyUp(); }

		FORCEINLINE void OnPositiveKeyUp() { positivePressed = false; if (!negativePressed) m_Amount = 0.0f; }
		FORCEINLINE void OnNegativeKeyUp() { negativePressed = false; if (!positivePressed) m_Amount = 0.0f; }

		FORCEINLINE float GetAmount() const { return m_Amount; }


	private:
		float m_Amount = 0.0f;
		bool positivePressed = false;
		bool negativePressed = false;
	};
}


#endif