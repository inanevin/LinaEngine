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

#pragma once

#ifndef ActionSubscriber_HPP
#define ActionSubscriber_HPP

#include "ActionDispatcher.hpp"
#include "Core/Internal.hpp"
#include "Utility/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace LinaEngine::Action
{
#define LINA_ACTION_CALLBACK(x) std::bind(&x, this)
#define LINA_ACTION_CALLBACK_PARAM1(x) std::bind(&x, this, std::placeholders::_1)
//#define BIND_ACTION_PARAM(x,y, z) [y](z i) { y->x(i); };

	class ActionSubscriber
	{
	public:

		ActionSubscriber() {};
		virtual ~ActionSubscriber();
		
		// Unsubscribes all handlers.
		void UnsubscribeAll();

		// Mutator for the target dispatcher.
		FORCEINLINE void SetActionDispatcher(ActionDispatcher* disp) { m_ActionDispatcher = disp; }

	protected:

		// Initialize the subscriber.
		virtual void Initialize() = 0;

		// Subscribes an action with particular id.
		void UnsubscribeAction(std::string actionID);

		// Subscribe an action with a condition.
		template<typename T>
		FORCEINLINE void SubscribeAction(const std::string& actionID, ActionType at, const std::function<void(T)>& callback, T condition)
		{
			// Abort if dispatcher is not set.
			if (!m_ActionDispatcher)
			{
				LINA_CORE_WARN("Dispatcher is not set yet, aborting subscription.");
				return;
			}

			size_t actionIDHashed = LinaEngine::Utility::StringToHash(actionID);

			// If an handler with the same id already exists, abort.
			if (m_Handlers.count(actionIDHashed) == 1)
			{
				LINA_CORE_WARN("The handler {0} already exists. Aborting subscription.", actionID);
				return;
			}

			// Create handler, assign condition if desired & callback.
			ActionHandler<T>* handler = new ActionHandler<T>(at);

			handler->SetCondition(condition);
			handler->SetCallback(callback);

			// Insert the handler to our map & subscribe to dispatcher.
			m_Handlers.insert(std::pair<size_t, ActionHandlerBase*>(actionIDHashed, handler));
			m_ActionDispatcher->SubscribeHandler(handler);
		}

		// Subscribe an action without a condition.
		template<typename T>
		FORCEINLINE void SubscribeAction(const std::string& actionID, ActionType at, const std::function<void(T)>& callback)
		{
			// Abort if dispatcher is not set.
			if (!m_ActionDispatcher)
			{
				LINA_CORE_WARN("Dispatcher is not set yet, aborting subscription.");
				return;
			}

			size_t actionIDHashed = LinaEngine::Utility::StringToHash(actionID);

			// If an handler with the same id already exists, abort.
			if (m_Handlers.count(actionIDHashed) == 1)
			{
				LINA_CORE_WARN("The handler {0} already exists. Aborting subscription.", actionID);
				return;
			}

			// Create handler, assign condition if desired & callback.
			ActionHandler<T>* handler = new ActionHandler<T>(at);

			handler->SetCallback(callback);

			// Insert the handler to our map & subscribe to dispatcher.
			m_Handlers.insert(std::pair<size_t, ActionHandlerBase*>(actionIDHashed, handler));
			m_ActionDispatcher->SubscribeHandler(handler);
		}

	private:

		// Target dispatcher.
		ActionDispatcher* m_ActionDispatcher = nullptr;

		// Handler map.
		std::map<size_t, ActionHandlerBase*> m_Handlers;
	};
}


#endif