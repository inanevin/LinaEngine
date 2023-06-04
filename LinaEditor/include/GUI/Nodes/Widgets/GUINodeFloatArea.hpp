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

#ifndef GUINodeFloatArea_HPP
#define GUINodeFloatArea_HPP

#include "GUI/Nodes/Widgets/GUINodeNumberArea.hpp"

namespace Lina::Editor
{
	class GUINodeFloatArea : public GUINodeNumberArea
	{
	public:
		GUINodeFloatArea(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeFloatArea(){};

		virtual void Draw(int threadID) override;
		virtual void OnTitleChanged(const String& str) override;

		inline void SetOwnValue(float val)
		{
			m_ptr	   = &m_ownValue;
			m_ownValue = val;
		}

	protected:
		virtual void UpdateTitle(int decimals) override;
		virtual void IncrementValue(const Vector2i& delta) override;
		virtual void OnStartedIncrementing() override;

		virtual String GetDefaultValueStr() override
		{
			return "0.0";
		}

	private:
		int	   m_maxDecimals		 = 5;
		int	   m_lastDecimals		 = 0;
		float* m_ptr				 = nullptr;
		float  m_lastUpdatedValue	 = 0.0f;
		float  m_ownValue			 = 0.0f;
		float  m_valOnIncrementStart = 0.0f;
	};

} // namespace Lina::Editor

#endif
