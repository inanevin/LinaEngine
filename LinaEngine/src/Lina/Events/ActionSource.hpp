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

#include "Lina/Events/Action.hpp"

namespace LinaEngine
{
#define BIND_ACTION(x,y) std::bind(&x, y)
#define BIND_ACTION_PARAM(x,y, z) [y](z i) { y->x(i); };

	template<typename T>
	class ActionParams
	{
	public:

		ActionParams() {};

		ActionParams(ActionType type, T c = NULL, std::function<void()> cb = NULL, std::function<void(T)> cbp = NULL, T* b = NULL)
			: actionType(type), condition(c), callback(cb), callbackWithParameter(cbp) {};

		ActionType actionType;
		T condition = NULL;
		std::function<void()> callback;
		std::function<void(T)> callbackWithParameter;
		T* binding = NULL;

	private:

		friend class IInputSubscriber;
		friend class ActionSource;
		void* caller;

	};

	class ActionSource
	{
	public:

		ActionSource() {};
		~ActionSource() {};

	private:

		friend class IInputSubscriber;

		template<typename T>
		void SubscribeToAction(const ActionParams<T>& params)
		{
			if (params.condition != NULL || params.binding != NULL)
			{
				// Init handler depending on param settings.
				ActionHandler<T>* handler = new ActionHandler<T>(params.actionType, params.caller);
				
				if (params.callback)
				{
					handler->SetNoParamCallback(params.callback);
					handler->SetUseNoParamCallback(true);
				}

				if (params.callbackWithParameter)
				{
					handler->SetUseParamCallback(true);
					handler->SetParamCallback(params.callbackWithParameter);
				}

				if (params.condition != NULL)
				{
					handler->SetUseCondition(true);
					handler->SetCondition(params.condition);
				}

				if (params.binding != NULL)
				{
					handler->SetUseBinding(true);
					handler->SetBinding(params.binding);
				}

				// Push it a shared ptr to the handler to the list.
				//m_Handlers.push_back(std::move(handler));

				m_ActionDispatcher.SubscribeHandler(handler);

			}
			else
			{
				ActionHandler<>* handler = new ActionHandler<>(params.actionType, params.caller);

				if (params.callback != NULL)
				{
					handler->SetUseNoParamCallback(true);
					handler->SetNoParamCallback(params.callback);
				}

				// Push it a shared ptr to the handler to the list.
				//m_Handlers.push_back(std::move(handler));

				m_ActionDispatcher.SubscribeHandler(handler);
			}

			// construct a weakptr out of the shared.
			//std::weak_ptr<ActionHandlerBase> wptr = *(std::prev(m_Handlers.end()));

			// Subscribe the handler.
			//m_ActionDispatcher.SubscribeHandler(wptr);
		}

		void UnsubscribeFromAction(void* addr)
		{
			m_ActionDispatcher.UnsubscribeHandler(addr);
		}

	protected:

		/* Subscribed action handlers. */
		std::list<ActionHandlerBase*> m_Handlers;

		/* Input dispatcher for actions. */
		ActionDispatcher m_ActionDispatcher;

	};
}


#endif