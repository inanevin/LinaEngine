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
	// Dispatcher class for actions.
	class ActionDispatcher
	{

	public:

		virtual ~ActionDispatcher();

	protected:

		ActionDispatcher();

		/* Dispatches the given action. */
		template<typename T>
		FORCEINLINE void DispatchAction(ActionType at, const T& data)
		{
			try {

				// Get the handler array corresponding to the action type.
				LinaArray<ActionHandlerBase*>& arr = m_ActionHandlerMap.at(at);

				// Iterate through the array of handlers w/ the same action type.
				LinaArray<ActionHandlerBase*>::iterator it;
				for (it = arr.begin(); it != arr.end(); it++)
				{
					ActionHandler<T>* handler = (static_cast<ActionHandler<T>*>(*it));
					handler->ControlExecute(data);
				}
			}
			catch (const LinaOutOfRange& e)
			{
				LINA_CORE_ERR("Out of Range Exception while subscribing handler!");
			}
		}

	private:

		friend class ActionSubscriber;

		/* Adds the handler to the list.*/
		void SubscribeHandler(ActionHandlerBase* ptr);

		/*  Removes the handler from the handlers list. */
		void UnsubscribeHandler(ActionHandlerBase* handler);

	private:

		LinaMap<uint32, LinaArray<ActionHandlerBase*>> m_ActionHandlerMap;

	};
}


#endif