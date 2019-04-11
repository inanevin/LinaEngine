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
	void* addr;
	ActionDispatcher::ActionDispatcher()
	{
		// For each action type insert a new list to the map.
		for (int i = 0; i < (ActionType::ACTION_TYPES_LASTINDEX + 1); i++)
		{
			m_ActionHandlerMap.insert(LinaMakePair(i, LinaList<ActionHandlerBase*>()));
		}
	}

	ActionDispatcher::~ActionDispatcher()
	{
		// Clear map, no ownership action.
		m_ActionHandlerMap.erase(m_ActionHandlerMap.begin(), m_ActionHandlerMap.end());

		//m_ActionHandlers.clear();
	}

	void ActionDispatcher::DispatchAction(ActionBase & action)
	{
		if (m_ActionHandlers.size() == 0) return;

		for (it = m_ActionHandlers.begin(); it != m_ActionHandlers.end(); it++)
		{
			// Check if the the object is alive. ( Technically, this check is usually pointless but heck it though )
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
		// Add the pointer to the list.
		m_ActionHandlers.push_back(ptr);
	}

	void ActionDispatcher::UnsubscribeHandler(ActionHandlerBase * handler)
	{
		m_ActionHandlers.remove(handler);
	}
}

