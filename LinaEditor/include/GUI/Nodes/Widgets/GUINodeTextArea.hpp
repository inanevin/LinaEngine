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

#ifndef GUINodeTextArea_HPP
#define GUINodeTextArea_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "GUI/Nodes/Layouts/GUINodeScrollArea.hpp"
#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Data/Bitmask.hpp"

namespace Lina
{
	class Input;
}

namespace LinaVG
{
	struct TextOutData;
}

namespace Lina::Editor
{
	class GUINodeText;

	class GUINodeTextArea : public GUINodeScrollArea
	{
	private:
		struct CharData
		{
			uint8 byteCount = 0;
		};

	public:
		GUINodeTextArea(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeTextArea();

		virtual void Draw(int threadID) override;
		virtual void OnPressBegin(uint32 button) override;
		virtual void OnLostFocus() override;
		virtual void SetTitle(const String& title) override;
		virtual void OnDoubleClicked() override;
		virtual void OnKey(uint32 key, InputAction act) override;
		virtual bool OnShortcut(Shortcut sc) override;
		virtual void OnChildExceededSize(float amt) override;

		virtual CursorType GetHoveredCursor() override
		{
			return CursorType::Caret;
		}

		inline Bitmask16& GetInputMask()
		{
			return m_inputMask;
		}

		inline void SetTextOffset(float val)
		{
			m_textOffset = val;
		}

		inline void SetAllowEditing(bool allow)
		{
			m_allowEditing = allow;
		}

		inline void SetExpandable(bool expandable)
		{
			m_expandable = expandable;
		}

	protected:
		void   DrawCaretAndSelection(int threadID);
		void   SetCaretFromMouse(bool isStartIndex);
		void   StartEditing();
		void   FinishEditing();
		void   EraseChar();
		void   SelectAll();
		void   FindPositions(uint32 min, uint32 max, uint32& posMin, uint32& posMax);
		void   CheckCaretIndexAndScroll(bool preferLeft);
		uint32 FindLineFromIndex(uint32 index);
		uint32 GetIndexInLine(uint32 line, float posX);

		virtual void   DrawBackground(int threadID);
		virtual String VerifyTitle(bool& titleOK);
		virtual void   OnTitleChanged(const String& val){};

	protected:
		LinaVG::TextOutData* m_outTextData				= nullptr;
		bool				 m_recalculateCharacterInfo = false;
		float				 m_lastWidth				= 0.0f;
		bool				 m_allowEditing				= true;
		bool				 m_expandable				= false;
		bool				 m_draggingExpandRect		= false;
		bool				 m_expandActive				= false;
		float				 m_additionalHeight			= 0.0f;
		String				 m_preEditTitle				= "";
		Bitmask16			 m_inputMask				= 0;
		Input*				 m_input					= nullptr;
		Vector<CharData>	 m_characters;
		bool				 m_isEditing			 = false;
		uint32				 m_caretIndexStart		 = 0;
		uint32				 m_caretIndexEnd		 = 0;
		uint32				 m_caretLineStart		 = 0;
		uint32				 m_caretLineEnd			 = 0;
		bool				 m_shouldDrawCaret		 = true;
		float				 m_caretTimer			 = 0.0f;
		float				 m_caretHeight			 = 0.0f;
		Vector2				 m_initialTextPos		 = Vector2::Zero;
		float				 m_minY					 = 0.0f;
		float				 m_textOffset			 = 0.0f;
		float				 m_wrapWidth			 = 0.0f;
		int					 m_caretScrollCheckState = 0;
	};

} // namespace Lina::Editor

#endif
