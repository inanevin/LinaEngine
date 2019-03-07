/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ActionTest
Timestamp: 3/3/2019 1:51:34 PM

*/

#include "LinaPch.hpp"
#include "ActionTest.hpp"  
#include "Lina/Input/InputEngine.hpp"

namespace LinaEngine
{
	ActionTest::ActionTest()
	{

	}

	void ActionTest::SetAction(InputEngine* inp)
	{
		ActionParams<int> params;
		
		params.actionType = ActionType::KeyPressed;
		params.condition = LINA_KEY_F3;
		params.callback = BIND_ACTION(ActionTest::Test, this);
		inp->SubscribeToAction(params);
	}

	void ActionTest::Test()
	{
		std::cout << "Test object callback is called! " << this << std::endl;
	}
}
