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
Class: ActionSubscriber

Subscriber instances are used to register particular action instances with conditions, function objects etc.
You can derive from the action subscriber or create an object to use it via composition.

Timestamp: 4/12/2019 3:36:21 AM
*/

#pragma once

#ifndef ActionSubscriber_HPP
#define ActionSubscriber_HPP

#include "ActionDispatcher.hpp"
#include "Utility/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace LinaEngine::Action
{
#define LINA_ACTION_CALLBACK(x) std::bind(&x, this)
#define LINA_ACTION_CALLBACK_PARAM1(x) std::bind(&x, this, std::placeholders::_1)

	class ActionSubscriber
	{
	public:

		ActionSubscriber() {};
		virtual ~ActionSubscriber();
		
		void UnsubscribeAll();

		FORCEINLINE void SetActionDispatcher(ActionDispatcher* disp) { m_targetDispatcher = disp; }

	protected:

		void UnsubscribeAction(std::string actionID);

		// Subscribe an action with a condition, action types must have an available default constructor.
		template<typename T>
		FORCEINLINE void SubscribeAction(const std::string& actionID, ActionType at, const std::function<void(T)>& callback, T condition)
		{
			// Abort if dispatcher is not set.
			if (!m_targetDispatcher)
			{
				LINA_CORE_WARN("Dispatcher is not set yet, aborting subscription.");
				return;
			}

			size_t actionIDHashed = LinaEngine::Utility::StringToHash(actionID);

			// If an handler with the same id already exists, abort.
			if (m_handlers.count(actionIDHashed) == 1)
			{
				LINA_CORE_WARN("The handler {0} already exists. Aborting subscription.", actionID);
				return;
			}

			// Create handler, assign condition if desired & callback.
			ActionHandler<T>* handler = new ActionHandler<T>(at);

			handler->SetCondition(condition);
			handler->SetCallback(callback);

			// Insert the handler to our map & subscribe to dispatcher.
			m_handlers.insert(std::pair<size_t, ActionHandlerBase*>(actionIDHashed, handler));
			m_targetDispatcher->SubscribeHandler(handler);
		}

		// Subscribe an action without a condition, action types must have an available default constructor.
		template<typename T>
		FORCEINLINE void SubscribeAction(const std::string& actionID, ActionType at, const std::function<void(T)>& callback)
		{
			// Abort if dispatcher is not set.
			if (!m_targetDispatcher)
			{
				LINA_CORE_WARN("Dispatcher is not set yet, aborting subscription.");
				return;
			}

			size_t actionIDHashed = LinaEngine::Utility::StringToHash(actionID);

			// If an handler with the same id already exists, abort.
			if (m_handlers.count(actionIDHashed) == 1)
			{
				LINA_CORE_WARN("The handler {0} already exists. Aborting subscription.", actionID);
				return;
			}

			// Create handler, assign condition if desired & callback.
			ActionHandler<T>* handler = new ActionHandler<T>(at);

			handler->SetCallback(callback);

			// Insert the handler to our map & subscribe to dispatcher.
			m_handlers.insert(std::pair<size_t, ActionHandlerBase*>(actionIDHashed, handler));
			m_targetDispatcher->SubscribeHandler(handler);
		}

	private:

		ActionDispatcher* m_targetDispatcher = nullptr;
		std::map<size_t, ActionHandlerBase*> m_handlers;

	};
}


#endif