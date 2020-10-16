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

#include "Actions/ActionSubscriber.hpp"  

namespace LinaEngine::Action
{
	ActionSubscriber::~ActionSubscriber()
	{
		UnsubscribeAll();
	}

	void ActionSubscriber::UnsubscribeAll()
	{
		// Abort if dispatcher is not set.
		if (!m_targetDispatcher)
		{
			LINA_CORE_WARN("Dispatcher is not set yet, aborting unsubscription. {0} ", typeid(*this).name());
			return;
		}

		for (std::map<size_t, ActionHandlerBase*>::iterator it = m_handlers.begin(); it != m_handlers.end(); it++)
		{
			m_targetDispatcher->UnsubscribeHandler(it->second);
			delete it->second;
		}

		m_handlers.clear();
	}

	void ActionSubscriber::UnsubscribeAction(std::string actionID)
	{
		// Abort if dispatcher is not set.
		if (!m_targetDispatcher)
		{
			LINA_CORE_WARN("Dispatcher is not set yet, aborting unsubscription. {0} {1}",actionID,  typeid(*this).name());
			return;
		}

		size_t actionIDHashed = Utility::StringToHash(actionID);

		// Abort if the handler does not exists.
		if (m_handlers.count(actionIDHashed) != 1)
		{
			LINA_CORE_WARN("No handler with the name {0} exists, aborting unsubscription. {1} ", actionID, typeid(*this).name());
			return;
		}

		m_targetDispatcher->UnsubscribeHandler(m_handlers.at(actionIDHashed));
		delete m_handlers.at(actionIDHashed);
		m_handlers.erase(actionIDHashed);
	}
}

