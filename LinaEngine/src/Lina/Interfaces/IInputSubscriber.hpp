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

#include "Lina/Input/InputEngine.hpp"

namespace LinaEngine
{
	class LINA_API IInputSubscriber
	{

	public:

		IInputSubscriber();
		~IInputSubscriber();

	protected:

		template<typename T>
		void SubscribeInputAction(ActionParams<T> params)
		{
			params.caller = this;
			inputEngine->SubscribeToAction(params);
		};

		void SubscribeKeyPressedAction();


		void SubscribeKeyReleasedAction();


		void SubscribeMousePressedAction();


		void SubscribeMouseReleasedAction();


	private:

		class InputEngine* inputEngine;
	};
}

#endif