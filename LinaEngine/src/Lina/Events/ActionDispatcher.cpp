/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ActionDispatcher
Timestamp: 4/10/2019 1:26:39 PM

*/

#include "LinaPch.hpp"
#include "ActionDispatcher.hpp"  

namespace LinaEngine
{
	ActionDispatcher::ActionDispatcher()
	{

	}

	ActionDispatcher::~ActionDispatcher()
	{
		m_ActionHandlers.clear();
	}

	void ActionDispatcher::DispatchAction(ActionBase & action)
	{
		if (m_ActionHandlers.size() == 0) return;

		for (it = m_ActionHandlers.begin(); it != m_ActionHandlers.end(); it++)
		{
			// Check if the the object is alive.
			if ((*it)->GetCaller() != NULL)
			{

				// Check if the action types match.
				if (action.GetActionType() == (*it)->GetActionType())
				{
					// Tell the handler to check the action.
					(*it)->Control(action);
				}
			}
			else
			{
				LINA_CORE_ERR("Fatal error in action source! Non-deleted action handler exists in handler list!");
			}
		}
	}

	void ActionDispatcher::SubscribeHandler(ActionHandlerBase * ptr)
	{
		// Add the weak pointer to the list.
		m_ActionHandlers.push_back(ptr);
	}

	void ActionDispatcher::UnsubscribeHandler(void * addr)
	{
		for (it = m_ActionHandlers.begin(); it != m_ActionHandlers.end();)
		{
			if ((*it)->GetCaller() == addr)
			{
				it = m_ActionHandlers.erase(it);
			}
			else
				++it;
		}
	}
}

