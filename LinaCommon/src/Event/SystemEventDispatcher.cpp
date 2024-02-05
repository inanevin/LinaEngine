/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Common/Event/SystemEventDispatcher.hpp"
#include "Common/Event/SystemEventListener.hpp"
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	SystemEventDispatcher::SystemEventDispatcher()
	{
		m_listeners.reserve(20);
	}

	void SystemEventDispatcher::AddListener(SystemEventListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void SystemEventDispatcher::RemoveListener(SystemEventListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](SystemEventListener* l) { return l == listener; }));
	}

	void SystemEventDispatcher::DispatchEvent(SystemEvent eventType, const Event& ev)
	{
		for (auto listener : m_listeners)
		{
			if (listener->GetSystemEventMask().IsSet(eventType))
				listener->OnSystemEvent(eventType, ev);
		}
	}
} // namespace Lina
