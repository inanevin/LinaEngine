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


namespace LinaEngine
{
	ActionTest::ActionTest()
	{
		std::cout << "TestObject CTOR With MemAddr: " << this << std::endl;
	}

	void ActionTest::SetAction()
	{
		ActionParams<Input::Key> params;
		params.actionType = ActionType::KeyPressed;
		//params.useCondition = true;
		params.condition = Input::Key::K;
		//params.useCallback = true;
		params.callback = LINA_ACTION_CALLBACK_PARAM1(ActionTest::TestParam);
		SubscribeInputAction(params);
		//auto bind = LINA_ACTION_CALLBACK(ActionTest::Test);
		//SubscribeKeyPressedAction(bind, Input::Key::Alpha0);
	}

	void ActionTest::Test()
	{
		std::cout << "Test object callback is called! " << this << std::endl;
	}
	void ActionTest::TestParam(Input::Key i)
	{
		std::cout << "Test object callback is called! " << static_cast<int>(i) << std::endl;
	}
}

