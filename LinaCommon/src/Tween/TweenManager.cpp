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

#include "Common/Tween/TweenManager.hpp"
#include "Common/System/SystemInfo.hpp"

namespace Lina
{
	TweenManager* TweenManager::s_instance = nullptr;

	TweenManager::~TweenManager()
	{
		for (auto* alloc : m_allocators)
			delete alloc;
	}

	Tween* TweenManager::AddTween(float* value, float start, float end, float duration, TweenType type)
	{
		LINA_ASSERT(SystemInfo::IsMainThread(), "");

		Tween* tw = nullptr;

		for (auto* alloc : m_allocators)
		{
			void* ptr = alloc->Allocate(sizeof(Tween), std::alignment_of<Tween>());

			if (ptr != nullptr)
			{
				tw				= new (ptr) Tween(value, start, end, duration, type);
				tw->m_allocator = alloc;
			}
		}

		if (tw == nullptr)
		{
			// Add new allocator
			PoolAllocator* alloc = new PoolAllocator(POOL_COUNT * sizeof(Tween), sizeof(Tween));
			alloc->Init();
			m_allocators.push_back(alloc);

			// Create
			void* ptr = alloc->Allocate(sizeof(Tween), std::alignment_of<Tween>());
			LINA_ASSERT(ptr != nullptr, "");
			tw				= new (ptr) Tween(value, start, end, duration, type);
			tw->m_allocator = alloc;
		}
		m_tweens.push_back(tw);
		return tw;
	}

	void TweenManager::Tick(float delta)
	{
		for (auto it = m_tweens.begin(); it != m_tweens.end();)
		{
			Tween* tw = *it;
			if (tw->Tick(delta))
			{
				it = m_tweens.erase(it);

				for (auto* t : tw->m_pendingTweens)
					t->m_waitingOn = nullptr;

				tw->m_allocator->Free(tw);
			}
			else
			{
				++it;
			}
		}
	}

} // namespace Lina
