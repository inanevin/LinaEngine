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

#ifndef GUINodeButton_HPP
#define GUINodeButton_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Data/String.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{

	enum class ButtonFitType
	{
		None,
		AutoFitFromTextAndPadding,
	};

	class GUINodeButton : public GUINode
	{
	public:
		GUINodeButton(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder){};
		virtual ~GUINodeButton() = default;

		virtual void	Draw(int threadID) override;
		virtual void	OnClicked(uint32 button) override;
		virtual Vector2 CalculateSize() override;

		inline void SetFontType(FontType ft)
		{
			m_fontType = ft;
		}

		inline void SetTextColor(const Color& color)
		{
			m_textColor = color;
		}

		inline void SetDefaultColor(const Color& col)
		{
			m_defaultColor = col;
		}

		inline void SetHoveredColor(const Color& col)
		{
			m_hoveredColor = col;
		}

		inline void SetPressedColor(const Color& col)
		{
			m_pressedColor = col;
		}

		inline void SetOutlineColor(const Color& col)
		{
			m_outlineColor = col;
		}

		inline void SetEnableHoverOutline(bool enableOutline)
		{
			m_enableHoverOutline = enableOutline;
		}

		inline void SetFitType(ButtonFitType ft)
		{
			m_fitType = ft;
		}

		inline void SetRounding(float rounding)
		{
			m_rounding = rounding;
		}

		inline void SetIsIcon(bool isIcon)
		{
			m_isIcon = isIcon;
		}

		inline void SetTextScale(float scale)
		{
			m_textScale = scale;
		}

	protected:
		float		  m_rounding		   = 0.1f;
		float		  m_textScale		   = 1.0f;
		bool		  m_enableHoverOutline = false;
		bool		  m_isIcon			   = false;
		ButtonFitType m_fitType			   = ButtonFitType::None;
		FontType	  m_fontType		   = FontType::DefaultEditor;
		Color		  m_defaultColor	   = Color::White;
		Color		  m_hoveredColor	   = Color::Gray;
		Color		  m_pressedColor	   = Color::Black;
		Color		  m_outlineColor	   = Color::Black;
		Color		  m_textColor		   = Color::White;
	};

} // namespace Lina::Editor

#endif
