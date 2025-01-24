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

#pragma once

#ifndef DataStructuresSyncQueue_HPP
#define DataStructuresSyncQueue_HPP

#include "Common/SizeDefinitions.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Data/String.hpp"
#include "Common/Data/Mutex.hpp"

namespace Lina
{
	struct SyncQueueElement
	{
		uint32			 id = 0;
		Vector<int>		 ints;
		Vector<float>	 floats;
		Vector<Vector2>	 vector2s;
		Vector<Vector2i> vector2is;
		Vector<String>	 strings;
	};

	class SyncQueue
	{
	public:
		using iterator		 = Vector<SyncQueueElement>::iterator;
		using const_iterator = Vector<SyncQueueElement>::const_iterator;

		iterator begin()
		{
			return m_elements.begin();
		}

		iterator end()
		{
			return m_elements.end();
		}

		const_iterator begin() const
		{
			return m_elements.begin();
		}

		const_iterator end() const
		{
			return m_elements.end();
		}

		void Push(const SyncQueueElement& el);
		void Push(uint32 id);
		void Push(uint32 id, float v);
		void Push(uint32 id, int v);
		void Push(uint32 id, const Vector2& v);
		void Push(uint32 id, const Vector2i& v);
		void Push(uint32 id, const String& v);
		void Reset();

		inline void SetMutex(Mutex* mtx)
		{
			m_mtx = mtx;
		}

	private:
		Mutex*					 m_mtx;
		Vector<SyncQueueElement> m_elements;
	};
} // namespace Lina

#endif
