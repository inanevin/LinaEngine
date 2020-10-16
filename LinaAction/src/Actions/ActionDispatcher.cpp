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

#include "Actions/ActionDispatcher.hpp"  
#include <stdexcept>

namespace LinaEngine::Action
{

	ActionDispatcher::ActionDispatcher()
	{
		// For each action type insert a new list to the map.
		for (int i = 0; i < (ActionType::ActionTypesLastIndex + 1); i++)
			m_actionHandlerMap.insert(std::make_pair(i, std::vector<ActionHandlerBase*>()));
	}

	ActionDispatcher::~ActionDispatcher()
	{		
		// Clear map, no ownership action.
		m_actionHandlerMap.clear();
	}

	void ActionDispatcher::SubscribeHandler(ActionHandlerBase* handler)
	{
		try 
		{
			// Add the pointer to the array.
			std::vector<ActionHandlerBase*>& arr = m_actionHandlerMap.at(handler->GetActionType());
			arr.push_back(handler);
		}
		catch (const std::out_of_range& e)
		{
			LINA_CORE_ERR("Out of Range Exception while subscribing handler! {0}", e.what());
		}

	}

	void ActionDispatcher::UnsubscribeHandler(ActionHandlerBase* handler)
	{
		try 
		{
			// Remove the pointer from the corresponding array.
			std::vector<ActionHandlerBase*>& arr = m_actionHandlerMap.at(handler->GetActionType());
			arr.erase(std::remove(arr.begin(), arr.end(), handler));
		}
		catch (const std::out_of_range& e)
		{
			LINA_CORE_ERR("Out of Range Exception while subscribing handler! {0}", e.what());
		}

	}
}

