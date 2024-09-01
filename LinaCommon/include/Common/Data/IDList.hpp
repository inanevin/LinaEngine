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

#include "Vector.hpp"
#include "Queue.hpp"
#include "Common/SizeDefinitions.hpp"
#include "Common/Log/Log.hpp"
#include "Common/Data/Streams.hpp"

namespace Lina
{
	template <typename T> class IDList
	{
	public:
		IDList(uint32 step, T defaultItem)
		{
			m_defaultItem = defaultItem;
			m_defaultStep = step;
			m_items.resize(m_defaultStep, defaultItem);
		}

		~IDList() = default;

		typename Vector<T>::iterator begin()
		{
			return m_items.begin();
		}

		typename Vector<T>::iterator end()
		{
			return m_items.end();
		}

		typename Vector<T>::const_iterator begin() const
		{
			return m_items.begin();
		}

		typename Vector<T>::const_iterator end() const
		{
			return m_items.end();
		}

		inline uint32 AddItem(T item)
		{
			uint32 id = 0;

			if (!m_availableIDs.empty())
			{
				id = m_availableIDs.front();
				m_availableIDs.pop();
			}
			else
				id = m_nextFreeID++;

			const uint32 currentSize = static_cast<uint32>(m_items.size());
			if (id >= currentSize)
				m_items.resize(m_defaultStep + currentSize);

			m_items[id] = item;
			return id;
		}

		inline void AddItem(T item, uint32 index)
		{
			m_items[index] = item;
		}

		inline void RemoveItem(uint32 id)
		{
			m_items[id] = m_defaultItem;
			m_availableIDs.push(id);
		}

		inline const Vector<T>& GetItems() const
		{
			return m_items;
		}

		inline Vector<T>& GetItems()
		{
			return m_items;
		}

		inline T GetItem(int index)
		{
			return m_items[index];
		}

		inline T& GetItemR(int index)
		{
			return m_items[index];
		}

		inline uint32 GetNextFreeID()
		{
			return m_nextFreeID;
		}

		inline void Clear()
		{
			m_items.clear();
		}

		inline void Reset()
		{
			Clear();
			m_items.resize(m_defaultStep, m_defaultItem);
		}

		inline T* GetRaw()
		{
			return m_items.data();
		}

	private:
		T			  m_defaultItem = T();
		Vector<T>	  m_items;
		Queue<uint32> m_availableIDs;
		uint32		  m_nextFreeID	= 0;
		uint32		  m_defaultStep = 250;
	};
} // namespace Lina
