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

#ifndef GUINodeNumberArea_HPP
#define GUINodeNumberArea_HPP

#include "GUI/Nodes/Widgets/GUINodeTextArea.hpp"

namespace Lina::Editor
{
	class GUINodeNumberArea : public GUINodeTextArea
	{
	public:
		GUINodeNumberArea(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeNumberArea(){};

		virtual String	   VerifyTitle(bool& titleOK) override;
		virtual void	   Draw(int threadID) override;
		virtual void	   DrawBackground(int threadID) override;
		virtual void	   OnPressBegin(uint32 button) override;
		virtual LinaGX::CursorType GetHoveredCursor() override;

		inline void SetHasLabelBox(bool hasLabelBox)
		{
			m_hasLabelBox = hasLabelBox;
		}

		inline void SetLabel(const String& lbl)
		{
			m_label = lbl;
		}

		inline void SetLabelColor(const Color& col)
		{
			m_labelColor = col;
		}

		inline void SetHasSlider(bool hasSlider)
		{
			m_hasSlider = hasSlider;
		}


	protected:
		virtual void   UpdateTitle(int decimals)			 = 0;
		virtual void   IncrementValue(const Vector2i& delta) = 0;
		virtual String GetDefaultValueStr()					 = 0;
		virtual void   OnStartedIncrementing()				 = 0;
		virtual void   SetValueFromPerc(float perc)			 = 0;
		virtual float  GetSliderPerc()						 = 0;

	protected:
		bool  m_hasLabelBox		 = false;
		bool  m_hasSlider		 = false;
		float m_sliderPercentage = 0.0f;

	private:
		Color  m_labelColor		  = Color::White;
		String m_label			  = "-";
		Rect   m_labelBoxRect	  = Rect();
		bool   m_draggingLabelBox = false;
		bool   m_draggingSlider	  = false;
	};

} // namespace Lina::Editor

#endif
