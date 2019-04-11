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
Timestamp: 4/10/2019 1:26:00 PM

*/

#pragma once

#ifndef ActionDispatcher_HPP
#define ActionDispatcher_HPP

#include "Action.hpp"

namespace LinaEngine
{

	template<typename T>
	class ActionParams
	{
	public:

		friend class IInputSubscriber;
		friend class IInputDispatcher;

		ActionParams() {};

		ActionParams(ActionType type, std::function<void(T)> cbp, T c = NULL)
			: actionType(type), condition(c), callback(cb) {};

		bool useCondition = false;
		ActionType actionType;
		T condition;
		std::function<void(T&)> callback;
		void* caller;

	};

	// Dispatcher class for actions.
	class ActionDispatcher
	{

	public:

		
		virtual ~ActionDispatcher();
		void operator=(ActionDispatcher const&) = delete;

	protected:

		ActionDispatcher();

		/* Dispatches the given action. */
		void DispatchAction(const ActionBase& action);

		/* Checks the action params & creates the necessary handle to pass into the internal subscription method. */
		template<typename T>
		FORCEINLINE void SubscribeToAction(const ActionParams<T>& params)
		{
			// Init handler depending on param settings.
			ActionHandler* handler = new ActionHandler<T>(params.actionType, params.caller);

			handler->SetCallback(params.callback);

			if (params.useCondition)
			{
				handler->SetCondition(params.condition);
			}

			m_ActionDispatcher.SubscribeHandler(handler);

		}

		/*  Removes the handler from the handlers list. */
		void UnsubscribeHandler(ActionHandlerBase* handler);

	private:

		/* Adds the handler to the list.*/
		void SubscribeHandler(ActionHandlerBase* ptr);

	private:

		LinaMap<uint32, LinaArray<ActionHandlerBase*>> m_ActionHandlerMap;

	};
}


#endif