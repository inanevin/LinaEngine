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

#include "Common/Tween/Tween.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	bool Tween::Tick(float delta)
	{
		if (Math::Equals(m_timeScale, 0.0f, 0.001f))
			return false;

		if (!m_passedDelay)
		{
			if (m_currentTime < m_delay)
			{
				m_currentTime += delta * m_timeScale;
				return false;
			}

			m_passedDelay = true;
			m_currentTime = 0.0f;
		}

		if (m_currentTime > m_duration)
		{
			m_value = m_end;
			if (m_restartCount == -1 || m_restarts < m_restartCount)
			{
				m_currentTime = 0.0f;
				m_passedDelay = false;
				// m_delay = 0.0f;
				m_restarts++;
				return false;
			}

			Complete();
			return true;
		}

		PerformTween();
		m_currentTime += delta * m_timeScale;

		return false;
	}

	void Tween::PerformTween()
	{
		if (m_currentTime > m_duration)
		{
			m_value = m_end;
			return;
		}

		const float t = m_currentTime / m_duration;
		switch (m_type)
		{
		case TweenType::Linear:
			m_value = Math::Linear(m_start, m_end, t);
			break;
		case TweenType::EaseIn:
			m_value = Math::EaseIn(m_start, m_end, t);
			break;
		case TweenType::EaseOut:
			m_value = Math::EaseOut(m_start, m_end, t);
			break;
		case TweenType::EaseInOut:
			m_value = Math::EaseInOut(m_start, m_end, t);
			break;
		case TweenType::Cubic:
			m_value = Math::Cubic(m_start, m_end, t);
			break;
		case TweenType::Sinusoidal:
			m_value = Math::Sinusodial(m_start, m_end, t);
			break;
		case TweenType::Exponential:
			m_value = Math::Exponential(m_start, m_end, t);
			break;
		case TweenType::Bounce:
			m_value = Math::Bounce(m_start, m_end, t);
			break;
		}

		if (m_onUpdate)
			m_onUpdate();
	}

	void Tween::Complete()
	{
		if (m_onCompleted)
			m_onCompleted();
		m_isCompleted = true;
	}
} // namespace Lina
