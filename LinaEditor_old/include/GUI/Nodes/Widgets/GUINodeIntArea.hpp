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

#ifndef GUINodeIntArea_HPP
#define GUINodeIntArea_HPP

#include "GUI/Nodes/Widgets/GUINodeNumberArea.hpp"

namespace Lina::Editor
{
	class GUINodeIntArea : public GUINodeNumberArea
	{
	public:
		GUINodeIntArea(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeIntArea(){};

		virtual void Draw(int threadID) override;
		virtual void OnTitleChanged(const String& str) override;

		inline void SetOwnValue(int val)
		{
			m_ownValue = val;
			SetPtr(&m_ownValue);
		}

		inline void SetPtr(int* ptr)
		{
			m_ptr = ptr;
		}

		inline void SetMinMax(int min, int max)
		{
			m_minValue = min;
			m_maxValue = max;
		}

	protected:
		virtual void  UpdateTitle(int decimals) override;
		virtual void  IncrementValue(const Vector2i& delta) override;
		virtual void  OnStartedIncrementing() override;
		virtual void  SetValueFromPerc(float perc) override;
		virtual float GetSliderPerc() override;

		virtual String GetDefaultValueStr() override
		{
			return "0";
		}

	private:
		int* m_ptr				   = nullptr;
		int	 m_ownValue			   = 0;
		int	 m_valOnIncrementStart = 0;
		int	 m_minValue			   = -99999;
		int	 m_maxValue			   = 99999;
	};

} // namespace Lina::Editor

#endif
