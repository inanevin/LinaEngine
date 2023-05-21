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

namespace Lina::Editor
{
	class GUINodeText;

	class GUINodeTextArea : public GUINodeScrollArea
	{
	private:
		struct CharData
		{
			float x			= 0.0f;
			float sizeX		= 0.0f;
			uint8 byteCount = 0;
		};

	public:
		GUINodeTextArea(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeTextArea() = default;

		virtual void Draw(int threadID) override;
		virtual void OnClicked(uint32 button) override;
		virtual void OnClickedOutside(uint32 button) override;
		virtual void SetTitle(const String& title) override;
		virtual void OnDragBegin() override;
		virtual void OnDoubleClicked() override;
		virtual void OnKey(uint32 key, InputAction act) override;
		virtual bool OnShortcut(Shortcut sc) override;
		virtual void OnChildExceededSize(float amt) override;

		inline Bitmask16& GetInputMask()
		{
			return m_inputMask;
		}

	protected:
		void   DrawCaretAndSelection(int threadID);
		uint32 FindCaretIndexFromMouse();
		void   FinishEditing();
		void   EraseChar();
		void   SelectAll();
		void   FindPositions(uint32 min, uint32 max, uint32& posMin, uint32& posMax);
		void   CheckCaretIndexAndScroll(bool preferLeft);

	protected:
		Bitmask16		 m_inputMask = CharacterMask::Printable;
		Input*			 m_input	 = nullptr;
		Vector<CharData> m_characters;
		bool			 m_isEditing	   = false;
		uint32			 m_caretIndexStart = 0;
		uint32			 m_caretIndexEnd   = 0;
		bool			 m_shouldDrawCaret = true;
		float			 m_caretTimer	   = 0.0f;
		float			 m_caretHeight	   = 0.0f;
		Vector2			 m_initialTextPos  = Vector2::Zero;
	};

} // namespace Lina::Editor

#endif
