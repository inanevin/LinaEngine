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

Class: ActionDispatcher

Dispatches the desired action (by template) with particular values. You can derive from the dispatcher,
or use it as an instance via composition.

Timestamp: 4/10/2019 1:26:00 PM

*/

#pragma once

#ifndef ActionDispatcher_HPP
#define ActionDispatcher_HPP

#include "Action.hpp"
#include "Utility/Log.hpp"
#include "Core/SizeDefinitions.hpp"
#include <map>
#include <vector>

namespace LinaEngine::Action 
{
	class ActionDispatcher
	{

	public:

		virtual ~ActionDispatcher();

	protected:

		ActionDispatcher();

		template<typename T>
		FORCEINLINE void DispatchAction(ActionType at, const T& data)
		{
			try {

				// Get the handler array corresponding to the action type.
				std::vector<ActionHandlerBase*>& arr = m_actionHandlerMap.at(at);

				// Iterate through the array of handlers w/ the same action type and execute to check conditions (if exists).
				std::vector<ActionHandlerBase*>::iterator it;
				for (it = arr.begin(); it != arr.end(); it++)
				{
					ActionHandler<T>* handler = (static_cast<ActionHandler<T>*>(*it));
					handler->ControlExecute(data);
				}
			}
			catch (const std::out_of_range& e)
			{
				const char* exp = e.what();
				LINA_CORE_ERR("Out of Range Exception while subscribing handler! {0}", exp);
			}
		}

	private:

		friend class ActionSubscriber;
		void SubscribeHandler(ActionHandlerBase* ptr);
		void UnsubscribeHandler(ActionHandlerBase* handler);

	private:

		std::map<uint32, std::vector<ActionHandlerBase*>> m_actionHandlerMap;

	};
}


#endif