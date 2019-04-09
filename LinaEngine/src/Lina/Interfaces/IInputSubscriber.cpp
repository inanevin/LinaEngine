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

#include "LinaPch.hpp"
#include "IInputSubscriber.hpp"  
#include "Lina/Input/InputEngine.hpp"
#include "Lina/Core/Application.hpp"

namespace LinaEngine
{
	IInputSubscriber::IInputSubscriber()
	{
		inputEngine = &Application::Get().GetInputEngine();
	}

	IInputSubscriber::~IInputSubscriber()
	{
		inputEngine->UnsubscribeFromAction(this);
	}

	/* KEY ACTIONS */

	/*void IInputSubscriber::SubscribeKeyPressedAction(std::function<void()> cb, Input::Key key)
	{
		ActionParams<Input::Key> params;

		if (key != Input::Key::Unknown)
			params.condition = key;

		params.actionType = ActionType::KeyPressed;
		params.callback = cb;
		SubscribeInputAction(params);
	}

	void IInputSubscriber::SubscribeKeyPressedAction(std::function<void(Input::Key)> cbp, Input::Key key)
	{
		ActionParams<Input::Key> params;
		
		if (key != Input::Key::Unknown)
			params.condition = key;

		params.actionType = ActionType::KeyPressed;
		params.callbackWithParameter = cbp;
		SubscribeInputAction(params);
	}

	void IInputSubscriber::SubscribeKeyPressedAction(std::function<void()> cb, std::function<void(Input::Key)> cbp, Input::Key key)
	{
		ActionParams<Input::Key> params;

		if (key != Input::Key::Unknown)
			params.condition = key;

		params.actionType = ActionType::KeyPressed;
		params.callback = cb;
		params.callbackWithParameter = cbp;
		SubscribeInputAction(params);
	}


	void IInputSubscriber::SubscribeKeyReleasedAction(std::function<void()> cb, Input::Key key)
	{
		ActionParams<Input::Key> params;

		if (key != Input::Key::Unknown)
			params.condition = key;

		params.actionType = ActionType::KeyReleased;
		params.callback = cb;
		SubscribeInputAction(params);
	}

	void IInputSubscriber::SubscribeKeyReleasedAction(std::function<void(Input::Key)> cbp, Input::Key key)
	{
		ActionParams<Input::Key> params;

		if (key != Input::Key::Unknown)
			params.condition = key;

		params.actionType = ActionType::KeyReleased;
		params.callbackWithParameter = cbp;
		SubscribeInputAction(params);
	}

	void IInputSubscriber::SubscribeKeyReleasedAction(std::function<void()> cb, std::function<void(Input::Key)> cbp, Input::Key key)
	{
		ActionParams<Input::Key> params;

		if (key != Input::Key::Unknown)
			params.condition = key;

		params.actionType = ActionType::KeyReleased;
		params.callback = cb;
		params.callbackWithParameter = cbp;
		SubscribeInputAction(params);
	}

	/* MOUSE BUTTON ACTIONS */
	/*
	void IInputSubscriber::SubscribeMouseButtonPressedAction(std::function<void()> cb, Input::Mouse button )
	{
		ActionParams<Input::Mouse> params;
		
		if (button != Input::Mouse::MouseUnknown)
			params.condition = button;

		params.actionType = ActionType::MouseButtonPressed;
		params.callback = cb;
		SubscribeInputAction(params);
	}

	void IInputSubscriber::SubscribeMouseButtonPressedAction(std::function<void(Input::Mouse)> cbp, Input::Mouse button)
	{
		ActionParams<Input::Mouse> params;

		if (button != Input::Mouse::MouseUnknown)
			params.condition = button;

		params.actionType = ActionType::MouseButtonPressed;
		params.callbackWithParameter = cbp;
		SubscribeInputAction(params);
	}

	void IInputSubscriber::SubscribeMouseButtonPressedAction(std::function<void()> cb, std::function<void(Input::Mouse)> cbp, Input::Mouse button)
	{
		ActionParams<Input::Mouse> params;

		if (button != Input::Mouse::MouseUnknown)
			params.condition = button;

		params.actionType = ActionType::MouseButtonPressed;
		params.callback = cb;
		params.callbackWithParameter = cbp;
		SubscribeInputAction(params);
	}


	void IInputSubscriber::SubscribeMouseButtonReleasedAction(std::function<void()> cb, Input::Mouse button)
	{
		ActionParams<Input::Mouse> params;

		if (button != Input::Mouse::MouseUnknown)
			params.condition = button;

		params.actionType = ActionType::MouseButtonReleased;
		params.callback = cb;
		SubscribeInputAction(params);
	}

	void IInputSubscriber::SubscribeMouseButtonReleasedAction(std::function<void(Input::Mouse)> cbp, Input::Mouse button)
	{
		ActionParams<Input::Mouse> params;

		if (button != Input::Mouse::MouseUnknown)
			params.condition = button;

		params.actionType = ActionType::MouseButtonReleased;
		params.callbackWithParameter = cbp;
		SubscribeInputAction(params);
	}

	void IInputSubscriber::SubscribeMouseButtonReleasedAction(std::function<void()> cb, std::function<void(Input::Mouse)> cbp, Input::Mouse button)
	{
		ActionParams<Input::Mouse> params;

		if (button != Input::Mouse::MouseUnknown)
			params.condition = button;

		params.actionType = ActionType::MouseButtonReleased;
		params.callback = cb;
		params.callbackWithParameter = cbp;
		SubscribeInputAction(params);
	}*/


}

