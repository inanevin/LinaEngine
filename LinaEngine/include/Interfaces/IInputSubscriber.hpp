/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: IInputSubscriber
Timestamp: 4/4/2019 2:29:37 AM

*/

#pragma once

#ifndef IInputSubscriber_HPP
#define IInputSubscriber_HPP


#include "Events/ActionSubscriber.hpp"
#include "Input/InputMappings.hpp"

namespace LinaEngine::Input
{
	class LINA_API IInputSubscriber : public ActionSubscriber
	{

	public:

		IInputSubscriber() { };
		virtual ~IInputSubscriber() {};

		void Initialize() override;

	protected:
		
		void SubscribeKeyPressedAction(const LinaString& handlerID, std::function<void(InputCode::Key)>&& cb, InputCode::Key key = InputCode::Key::Unknown)
		{
			if (key != InputCode::Key::Unknown)
				SubscribeAction<InputCode::Key>(handlerID, ActionType::KeyPressed, cb, key);
			else
				SubscribeAction<InputCode::Key>(handlerID, ActionType::KeyPressed, cb);
		}

		void SubscribeKeyReleasedAction(const LinaString& handlerID, std::function<void(InputCode::Key)>&& cb, InputCode::Key key = InputCode::Key::Unknown)
		{
			if (key != InputCode::Key::Unknown)
				SubscribeAction<InputCode::Key>(handlerID, ActionType::KeyReleased, cb, key);
			else
				SubscribeAction<InputCode::Key>(handlerID, ActionType::KeyReleased, cb);
		}

		void SubscribeMouseButtonPressedAction(const LinaString& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse button = InputCode::Mouse::MouseUnknown)
		{
			if (button != InputCode::Mouse::MouseUnknown)
				SubscribeAction<InputCode::Mouse>(handlerID, ActionType::MouseButtonPressed, cb, button);
			else
				SubscribeAction<InputCode::Mouse>(handlerID, ActionType::MouseButtonPressed, cb);
		}

		void SubscribeMouseButtonReleasedAction(const LinaString& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse button = InputCode::Mouse::MouseUnknown)
		{
			if (button != InputCode::Mouse::MouseUnknown)
				SubscribeAction<InputCode::Mouse>(handlerID, ActionType::MouseButtonReleased, cb, button);
			else
				SubscribeAction<InputCode::Mouse>(handlerID, ActionType::MouseButtonReleased, cb);
		}

	private:

	};
}

#endif