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

#include "Common/Data/SyncQueue.hpp"

namespace Lina
{
	void SyncQueue::Push(const SyncQueueElement& el)
	{
		LOCK_GUARD(*m_mtx);
		m_elements.push_back(el);
	}

	void SyncQueue::Push(uint32 id)
	{
		SyncQueueElement el;
		el.id = id;
		Push(el);
	}

	void SyncQueue::Push(uint32 id, float v)
	{
		SyncQueueElement el;
		el.id = id;
		el.floats.push_back(v);
		Push(el);
	}

	void SyncQueue::Push(uint32 id, int v)
	{
		SyncQueueElement el;
		el.id = id;
		el.ints.push_back(v);
		Push(el);
	}

	void SyncQueue::Push(uint32 id, const Vector2& v)
	{
		SyncQueueElement el;
		el.id = id;
		el.vector2s.push_back(v);
		Push(el);
	}

	void SyncQueue::Push(uint32 id, const Vector2i& v)
	{
		SyncQueueElement el;
		el.id = id;
		el.vector2is.push_back(v);
		Push(el);
	}

	void SyncQueue::Push(uint32 id, const String& v)
	{
		SyncQueueElement el;
		el.id = id;
		el.strings.push_back(v);
		Push(el);
	}
	void SyncQueue::Reset()
	{
		m_elements.clear();
	}
} // namespace Lina
