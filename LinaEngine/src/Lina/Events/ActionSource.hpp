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
#define BIND_ACTION_PARAM(x,y) [y](int i) { y->x(i); };

	template<typename T>
	class ActionParams
	{
	public:

		ActionParams() {};
		std::weak_ptr<void*> caller;
		ActionType actionType;
		T condition = NULL;
		std::function<void()> callback;
		std::function<void(T)> callbackWithParameter;
		T* binding = NULL;
	};

	class ActionSource
	{
	public:

		ActionSource() {};
		~ActionSource() {};

		template<typename T>
		void SubscribeToAction(ActionParams<T> params)
		{
			if (params.condition != NULL || params.binding != NULL)
			{
				// Init handler depending on param settings.
				ActionHandler<T> handler(params.actionType);
				
				if (params.callback)
				{
					handler.SetNoParamCallback(params.callback);
					handler.SetUseNoParamCallback(true);
				}

				if (params.callbackWithParameter)
				{
					handler.SetUseParamCallback(true);
					handler.SetParamCallback(params.callbackWithParameter);
				}

				if (params.condition != NULL)
				{
					handler.SetUseCondition(true);
					handler.SetCondition(params.condition);
				}

				if (params.binding != NULL)
				{
					handler.SetUseBinding(true);
					handler.SetBinding(params.binding);
				}

				// Push it a shared ptr to the handler to the list.
				m_Handlers.push_back(std::make_shared<ActionHandler<T>>(handler));

			}
			else
			{
				ActionHandler<> handler(params.actionType);

				if (params.callback != NULL)
				{
					handler.SetUseNoParamCallback(true);
					handler.SetNoParamCallback(params.callback);
				}

				// Push it a shared ptr to the handler to the list.
				m_Handlers.push_back(std::make_shared<ActionHandler<>>(handler));

			
			}

			// construct a weakptr out of the shared.
			std::weak_ptr<ActionHandlerBase> wptr = *(std::prev(m_Handlers.end()));

			// Subscribe the handler.
			m_ActionDispatcher.SubscribeHandler(wptr);
		}

	protected:

		/* Subscribed action handlers. */
		std::list<std::shared_ptr<ActionHandlerBase>> m_Handlers;

		/* Input dispatcher for actions. */
		ActionDispatcher m_ActionDispatcher;

	};
}


#endif