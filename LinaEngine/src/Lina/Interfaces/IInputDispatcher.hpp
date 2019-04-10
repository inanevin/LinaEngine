/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ActionSource
Timestamp: 3/2/2019 7:13:07 PM

*/

#pragma once

#ifndef ActionSource_HPP
#define ActionSource_HPP

#include "Lina/Events/ActionDispatcher.hpp"

namespace LinaEngine
{
#define LINA_ACTION_CALLBACK(x) std::bind(&x, this)
#define LINA_ACTION_CALLBACK_PARAM1(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_ACTION_PARAM(x,y, z) [y](z i) { y->x(i); };


	class IInputDispatcher : public ActionDispatcher
	{

		IInputDispatcher() {};
		~IInputDispatcher() {};

	public:

	protected:

	private:

	};
}


#endif