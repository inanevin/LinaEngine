/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ActionSubscriber
Timestamp: 4/12/2019 3:36:21 AM

*/

#include "Actions/ActionSubscriber.hpp"  

namespace LinaEngine::Action
{
	ActionSubscriber::~ActionSubscriber()
	{
		UnsubscribeAll();
	}

	void ActionSubscriber::UnsubscribeAll()
	{
		// Abort if dispatcher is not set.
		if (!m_ActionDispatcher)
		{
			LINA_CORE_WARN("Dispatcher is not set yet, aborting unsubscription. {0} ", typeid(*this).name());
			return;
		}

		for (std::map<size_t, ActionHandlerBase*>::iterator it = m_Handlers.begin(); it != m_Handlers.end(); it++)
		{
			m_ActionDispatcher->UnsubscribeHandler(it->second);
			delete it->second;
		}

		m_Handlers.clear();
	}

	void ActionSubscriber::UnsubscribeAction(std::string actionID)
	{
		// Abort if dispatcher is not set.
		if (!m_ActionDispatcher)
		{
			LINA_CORE_WARN("Dispatcher is not set yet, aborting unsubscription. {0} {1}",actionID,  typeid(*this).name());
			return;
		}

		size_t actionIDHashed = Utility::StringToHash(actionID);

		// Abort if the handler does not exists.
		if (m_Handlers.count(actionIDHashed) != 1)
		{
			LINA_CORE_WARN("No handler with the name {0} exists, aborting unsubscription. {1} ", actionID, typeid(*this).name());
			return;
		}

		m_ActionDispatcher->UnsubscribeHandler(m_Handlers.at(actionIDHashed));
		delete m_Handlers.at(actionIDHashed);
		m_Handlers.erase(actionIDHashed);
	}
}

