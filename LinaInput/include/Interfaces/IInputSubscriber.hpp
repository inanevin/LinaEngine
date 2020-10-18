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
Class: IInputSubscriber

A bridge to listen to any key & mouse events easily.

Timestamp: 4/4/2019 2:29:37 AM
*/

#pragma once

#ifndef IInputSubscriber_HPP
#define IInputSubscriber_HPP

#include "Input/InputMappings.hpp"
#include <string>
#include <functional>

namespace LinaEngine::Input
{
	class IInputSubscriber
	{

	public:

		IInputSubscriber() { };
		virtual ~IInputSubscriber() {};

	protected:
		
		void SubscribeKeyPressedAction(const std::string& handlerID, std::function<void(InputCode::Key)>&& cb, InputCode::Key key = InputCode::Key::Unknown);
		void SubscribeKeyReleasedAction(const std::string& handlerID, std::function<void(InputCode::Key)>&& cb, InputCode::Key key = InputCode::Key::Unknown);
		void SubscribeMouseButtonPressedAction(const std::string& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse button = InputCode::Mouse::MouseUnknown);
		void SubscribeMouseButtonReleasedAction(const std::string& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse button = InputCode::Mouse::MouseUnknown);
		void SubscribeMousePressedAction(const std::string& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse mouse = InputCode::Mouse::MouseUnknown);
		void SubscribeMouseReleasedAction(const std::string& handlerID, std::function<void(InputCode::Mouse)>&& cb, InputCode::Mouse mouse = InputCode::Mouse::MouseUnknown);
		

	private:

	};
}

#endif