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

#ifndef GUINodeText_HPP
#define GUINodeText_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "Math/Color.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{

	class GUINodeText : public GUINode
	{
	public:
		GUINodeText(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeText() = default;

		virtual void	Draw(int threadID) override;
		virtual Vector2 CalculateSize() override;

		inline void SetColorStart(const Color& col)
		{
			m_startColor = col;
		}

		inline void SetColorEnd(const Color& col)
		{
			m_endColor = col;
		}

		inline void SetBothColors(const Color& col)
		{
			m_startColor = m_endColor = col;
		}

		inline void SetFont(FontType fontType)
		{
			m_fontType = fontType;
		}

		inline void SetAlignment(TextAlignment align)
		{
			m_alignment = align;
		}

		inline void SetScale(float scale)
		{
			m_scale = scale;
		}

		inline void SetText(const char* text)
		{
			m_text	  = text;
			m_lastDpi = 0.0f;
			CalculateSize();
		}

		inline void SetText(const String& text)
		{
			m_text	  = text;
			m_lastDpi = 0.0f;
			CalculateSize();
		}

	protected:
		String		  m_text	   = "";
		float		  m_scale	   = 1.0f;
		TextAlignment m_alignment  = TextAlignment::Left;
		FontType	  m_fontType   = FontType::DefaultEditor;
		Color		  m_startColor = Color::White;
		Color		  m_endColor   = Color::White;
	};

	class GUINodeTextRichColors : public GUINodeText
	{
	private:
		struct TextData
		{
			String	text		   = "";
			Color	color		   = Color::White;
			Vector2 calculatedSize = Vector2::Zero;
		};

	public:
		GUINodeTextRichColors(GUIDrawerBase* drawer, int drawOrder) : GUINodeText(drawer, drawOrder){};
		virtual ~GUINodeTextRichColors() = default;

		virtual void	Draw(int threadID) override;
		virtual Vector2 CalculateSize() override;

		void AddText(const String& text, const Color& col);

	private:
		Vector<TextData> m_textData;
	};

} // namespace Lina::Editor

#endif
