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

namespace LinaEngine::Action
{
	
	void ActionDispatcher::Initialize(int startIndex, int endIndex)
	{
		// For each action type insert a new list to the map.
		for (int i = startIndex; i < (endIndex + 1); i++)
			m_actionHandlerMap.insert(std::make_pair(i, std::list<ActionHandlerBase*>()));
	}

	ActionDispatcher::~ActionDispatcher()
	{		
		// Free memory.
		for (std::map<uint32, std::list<ActionHandlerBase*>>::iterator it = m_actionHandlerMap.begin(); it != m_actionHandlerMap.end(); ++it)
		{
			for (std::list<ActionHandlerBase*>::iterator listIT = it->second.begin(); listIT != it->second.end(); ++listIT)
			{
				delete *listIT;
			}

			it->second.clear();
		}
		m_actionHandlerMap.clear();
	}


	void ActionDispatcher::UnsubscribeAction(const std::string& actionID, ActionType actionType)
	{
		std::list<ActionHandlerBase*>& targetList = m_actionHandlerMap.at(actionType);
		ActionHandlerBase* base = FindAction(targetList, actionID);
		
		if (base != nullptr)
		{
			delete base;
			targetList.remove(base);
		}
		else
			LINA_CORE_WARN("This action ID {0} does not exists, aborting unsubscription.", actionID);
	}

	ActionHandlerBase* ActionDispatcher::FindAction(std::list<ActionHandlerBase*>& targetList, const std::string& actionID)
	{
		size_t actionIDHashed = LinaEngine::Utility::StringToHash(actionID);
		std::list<ActionHandlerBase*>::iterator it = std::find_if(targetList.begin(), targetList.end(), [actionIDHashed]
		(const ActionHandlerBase* m) -> bool { return m->GetHashedID() == actionIDHashed; });

		return it != targetList.end() ? *it : nullptr;
	}
}

