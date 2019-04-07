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

		friend class IInputSubscriber;
		friend class ActionSource;

		ActionParams() {};

		ActionParams(ActionType type, T c = NULL, std::function<void()> cb = NULL, std::function<void(T)> cbp = NULL, T* b = NULL)
			: actionType(type), condition(c), callback(cb), callbackWithParameter(cbp) {};

		bool useCondition = false;
		bool useCallback = false;
		bool useParamCallback = false;
		bool useBinding = false;
		ActionType actionType;
		T condition;
		std::function<void()> callback;
		std::function<void(T)> callbackWithParameter;
		T* binding;
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
			if (params.useCondition || params.useBinding || params.useParamCallback)
			{
				// Init handler depending on param settings.
				ActionHandler<T>* handler = new ActionHandler<T>(params.actionType, params.caller);
				
				if (params.useCallback)
				{
					handler->SetNoParamCallback(params.callback);
					handler->SetUseNoParamCallback(true);
				}

				if (params.useParamCallback)
				{
					handler->SetUseParamCallback(true);
					handler->SetParamCallback(params.callbackWithParameter);
				}

				if (params.useCondition)
				{
					handler->SetUseCondition(true);
					handler->SetCondition(params.condition);
				}

				if (params.useBinding)
				{
					handler->SetUseBinding(true);
					handler->SetBinding(params.binding);
				}


				m_ActionDispatcher.SubscribeHandler(handler);

			}
			else
			{
				ActionHandler<>* handler = new ActionHandler<>(params.actionType, params.caller);

				if (params.useCallback)
				{
					handler->SetUseNoParamCallback(true);
					handler->SetNoParamCallback(params.callback);
				}

				m_ActionDispatcher.SubscribeHandler(handler);
			}


		}

		void UnsubscribeFromAction(void* addr)
		{
			m_ActionDispatcher.UnsubscribeHandler(addr);
		}

	protected:


		/* Input dispatcher for actions. */
		ActionDispatcher m_ActionDispatcher;

	};
}


#endif