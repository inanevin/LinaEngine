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

#include "Common/Data/Vector.hpp"
#include "Common/Data/Functional.hpp"
#include <memoryallocators/PoolAllocator.h>

namespace Lina
{
	enum class TweenType
	{
		Linear,
		EaseIn,
		EaseOut,
		EaseInOut,
		Cubic,
		Sinusoidal,
		Exponential,
		Bounce
	};

	class Tween
	{
	public:
		inline Tween* SetOnCompleted(Delegate<void()>&& onCompleted)
		{
			m_onCompleted = onCompleted;
			return this;
		}

		inline Tween* SetDelay(float delay)
		{
			m_delay = delay;
			return this;
		}

		inline Tween* AddPending(Tween* other)
		{
			m_pendingTweens.push_back(other);
			other->m_waitingOn = this;
			return this;
		}

		inline Tween* SetRestarts(int32 count)
		{
			m_restartCount = count;
			return this;
		}

		inline Tween* SetTimeScale(float ts)
		{
			m_timeScale = ts;
			return this;
		}

		inline Tween* Stop()
		{
			m_stopped = false;
			return this;
		}

		inline Tween* ResetTime()
		{
			m_currentTime = 0.0f;
			return this;
		}

		inline Tween* SetPersistent(bool isPersistent)
		{
			m_isPersistent = isPersistent;
			return this;
		}

	private:
		friend class TweenManager;
		Tween(float* value, float start, float end, float duration, TweenType type) : m_value(value), m_start(start), m_end(end), m_duration(duration), m_type(type){};
		~Tween() = default;
		bool Tick(float delta);
		void Complete();
		void PerformTween();

	private:
		TweenType		 m_type = TweenType::Linear;
		Delegate<void()> m_onCompleted;
		float*			 m_value		= nullptr;
		float			 m_start		= 0.0f;
		float			 m_end			= 0.0f;
		float			 m_delay		= 0.0f;
		float			 m_duration		= 0.0f;
		float			 m_currentTime	= 0.0f;
		int32			 m_restarts		= 0;
		int32			 m_restartCount = 0;

		PoolAllocator* m_allocator	  = nullptr;
		Tween*		   m_waitingOn	  = nullptr;
		bool		   m_passedDelay  = false;
		bool		   m_stopped	  = false;
		bool		   m_isPersistent = false;
		float		   m_timeScale	  = 1.0f;
		Vector<Tween*> m_pendingTweens;
	};
} // namespace Lina
