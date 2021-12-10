/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: IInputSubscriber
Timestamp: 10/18/2020 2:53:28 PM

*/

#include "Interfaces/IInputSubscriber.hpp"
#include "Input/InputEngine.hpp"
#include "Actions/Action.hpp"

namespace Lina::Input
{
		void IInputSubscriber::SubscribeKeyPressedAction(const std::string& handlerID, std::function<void(InputCode::Key)>&& cb, InputCode::Key key)
		{
			if (key != InputCode::Key::Unknown)
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Key>(handlerID, Lina::Action::ActionType::KeyPressed, cb, key);
			else
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Key>(handlerID, Lina::Action::ActionType::KeyPressed, cb);
		}

		void IInputSubscriber::SubscribeKeyReleasedAction(const std::string& handlerID, std::function<void(InputCode::Key)>&& cb, InputCode::Key key)
		{
			if (key != InputCode::Key::Unknown)
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Key>(handlerID, Lina::Action::ActionType::KeyReleased, cb, key);
			else
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Key>(handlerID, Lina::Action::ActionType::KeyReleased, cb);
		}

		void IInputSubscriber::SubscribeMouseButtonPressedAction(const std::string& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse button)
		{
			if (button != InputCode::Mouse::MouseUnknown)
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Mouse>(handlerID, Lina::Action::ActionType::MouseButtonPressed, cb, button);
			else
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Mouse>(handlerID, Lina::Action::ActionType::MouseButtonPressed, cb);
		}

		void IInputSubscriber::SubscribeMouseButtonReleasedAction(const std::string& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse button)
		{
			if (button != InputCode::Mouse::MouseUnknown)
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Mouse>(handlerID, Lina::Action::ActionType::MouseButtonReleased, cb, button);
			else
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Mouse>(handlerID, Lina::Action::ActionType::MouseButtonReleased, cb);
		}

		void IInputSubscriber::SubscribeMousePressedAction(const std::string& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse mouse )
		{
			if (mouse != InputCode::Mouse::MouseUnknown)
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Mouse>(handlerID, Lina::Action::ActionType::MouseButtonPressed, cb, mouse);
			else
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Mouse>(handlerID, Lina::Action::ActionType::MouseButtonPressed, cb);
		}

		void IInputSubscriber::SubscribeMouseReleasedAction(const std::string& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse mouse )
		{
			if (mouse != InputCode::Mouse::MouseUnknown)
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Mouse>(handlerID, Lina::Action::ActionType::MouseButtonReleased, cb, mouse);
			else
				InputEngine::GetInputDispatcher().SubscribeAction<InputCode::Mouse>(handlerID, Lina::Action::ActionType::MouseButtonReleased, cb);
		}
}