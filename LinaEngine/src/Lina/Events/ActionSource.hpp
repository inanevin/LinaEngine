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
#define LINA_ACTION_CALLBACK(x) std::bind(&x, this)
#define LINA_ACTION_CALLBACK_PARAM1(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_ACTION_PARAM(x,y, z) [y](z i) { y->x(i); };


	template<typename T>
	class ActionParams
	{
	public:

		friend class IInputSubscriber;
		friend class ActionSource;

		ActionParams() {};

		ActionParams(ActionType type, T c = NULL, std::function<void(T)> cbp = NULL, T* b = NULL)
			: actionType(type), condition(c), callback(cb), callbackWithParameter(cbp) {};

		bool useCondition = false;
		bool useCallback = false;
		bool useBinding = false;
		ActionType actionType;
		T condition;
		std::function<void(T&)> callback;
		T* binding;
		void* caller;

	};

	class ActionSource
	{
	public:

	private:

		friend class IInputSubscriber;

		template<typename T>
		void SubscribeToAction(const ActionParams<T>& params)
		{
			// Init handler depending on param settings.
			ActionHandler<T>* handler = new ActionHandler<T>(params.actionType, params.caller);

			if (params.useCallback)
			{
				handler->SetCallback(params.callback);
			}

			if (params.useCondition)
			{
				handler->SetCondition(params.condition);
			}

			if (params.useBinding)
			{
				handler->SetBinding(params.binding);
			}

			m_ActionDispatcher.SubscribeHandler(handler);

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