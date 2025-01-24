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

#include "Common/Data/CommonData.hpp"
#include "Common/Data/IDList.hpp"
#include "Common/Data/Queue.hpp"
#include "Common/Memory/CommonMemory.hpp"
#include <type_traits>
#include <new>

namespace Lina
{
	// T must have BucketIdentifier m_bucketIdent member variable.
	// T must be default constructable.
	template <typename T, int N> class AllocatorBucket
	{
	public:
		static constexpr size_t ALIGN = std::alignment_of<T>::value;

		AllocatorBucket(uint8 bucketIndex = 0)
		{
			static_assert(N < UINT16_MAX, "Max pool count is max uint16!");

			m_bucketIndex = bucketIndex;

			const size_t alignedSize = GetAlignedSize();
			m_rawMemory				 = new uint8[alignedSize];
			MEMSET(m_rawMemory, 0, alignedSize);
			AdjustSpan(alignedSize);
		};

		~AllocatorBucket()
		{
			if (m_nextBucket != nullptr)
				delete m_nextBucket;

			for (uint32 i = 0; i < N; i++)
			{
				uint8* ptr = m_span.data() + i * sizeof(T);
				T*	   obj = reinterpret_cast<T*>(ptr);

				if (obj->m_bucketIdent.isValid == 0)
					continue;

				obj->~T();
			}

			delete[] m_rawMemory;
		};

		void Clear()
		{
			m_head	  = 0;
			m_freeIDs = Queue<uint16>();

			if (m_nextBucket != nullptr)
				m_nextBucket->Clear();
		}

		template <typename... Args> T* Allocate(Args&&... args)
		{
			uint16 target = 0;

			if (!m_freeIDs.empty())
			{
				target = m_freeIDs.front();
				m_freeIDs.pop();
			}
			else
			{
				target = m_head;

				if (m_head < N)
					m_head++;
			}

			// Simple grow policy.
			if (target >= N)
			{
				if (m_nextBucket != nullptr)
					return m_nextBucket->Allocate(std::forward<Args>(args)...);

				m_nextBucket = new AllocatorBucket<T, N>(m_bucketIndex + 1);
				return m_nextBucket->Allocate(std::forward<Args>(args)...);
			}

			const uint8* ptr   = m_span.data() + target * sizeof(T);
			T*			 obj   = new ((void*)ptr) T(args...);
			obj->m_bucketIdent = {
				.allocationIndex = target,
				.bucketIndex	 = m_bucketIndex,
				.isValid		 = 1,
			};
			return obj;
		}

		void Free(T* obj)
		{
			if (obj->m_bucketIdent.bucketIndex != m_bucketIndex)
			{
				m_nextBucket->Free(obj);
				return;
			}

			const uint16 targetId = obj->m_bucketIdent.allocationIndex;
			m_freeIDs.push(targetId);
			obj->m_bucketIdent = {
				.allocationIndex = 0,
				.bucketIndex	 = 0,
				.isValid		 = 0,
			};

			obj->~T();
		};

		void View(Delegate<bool(T* item, uint32 index)>&& callback, uint32 startIdx = 0) const
		{
			uint32 idx = startIdx;
			for (uint32 i = 0; i < N; i++)
			{
				uint8* ptr = m_span.data() + i * sizeof(T);
				T*	   obj = reinterpret_cast<T*>(ptr);

				if (obj->m_bucketIdent.isValid == 0)
					continue;

				if (callback(obj, idx))
					return;
				idx++;
			}

			if (m_nextBucket != nullptr)
				m_nextBucket->View(std::move(callback), idx);
		}

		T* Find(Delegate<bool(T* inst)> predicate) const
		{
			for (uint32 i = 0; i < N; i++)
			{
				uint8* ptr = m_span.data() + i * sizeof(T);
				T*	   obj = reinterpret_cast<T*>(ptr);

				if (obj->m_bucketIdent.isValid == 0)
					continue;

				if (predicate(obj))
					return obj;
			}

			if (m_nextBucket != nullptr)
				return m_nextBucket->Find(std::move(predicate));

			return nullptr;
		}

		uint32 GetActiveItemCount(uint32 prior = 0) const
		{
			uint32 count = prior;

			for (uint32 i = 0; i < N; i++)
			{
				uint8* ptr = m_span.data() + i * sizeof(T);
				T*	   obj = reinterpret_cast<T*>(ptr);

				if (obj->m_bucketIdent.isValid == 0)
					continue;
				count++;
			}

			if (m_nextBucket != nullptr)
				return m_nextBucket->GetActiveItemCount(count);

			return count;
		}

		uint32 GetElement(uint32 id, uint32 startIdx = 0) const
		{
			uint32 idx = startIdx;
		}

	private:
		size_t GetAlignedSize()
		{
			const size_t rawSize = N * sizeof(T);
			return (rawSize + ALIGN - 1) & ~(ALIGN - 1);
		}

		void AdjustSpan(size_t alignedSize)
		{
			size_t adjustment = (ALIGN - (reinterpret_cast<uintptr_t>(m_rawMemory) & (ALIGN - 1))) & (ALIGN - 1);
			m_span			  = {m_rawMemory + adjustment, alignedSize - adjustment};
		}

	private:
		Queue<uint16> m_freeIDs;
		Span<uint8>	  m_span;
		uint8*		  m_rawMemory	= nullptr;
		uint16		  m_head		= 0;
		uint8		  m_bucketIndex = 0;

		AllocatorBucket<T, N>* m_nextBucket = nullptr;
	};
} // namespace Lina
