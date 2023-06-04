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

#include "GUI/Nodes/Widgets/GUINodeTextArea.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "GUI/Drawers/GUIDrawerBase.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "GUI/Nodes/Layouts/GUINodeScrollArea.hpp"
#include "GUI/Nodes/Widgets/GUINodeText.hpp"
#include "Core/Theme.hpp"
#include "Commands/EditorCommandWidgets.hpp"
#include "Math/Math.hpp"
#include "Input/Core/InputMappings.hpp"
#include "Input/Core/Input.hpp"
#include "Core/SystemInfo.hpp"
#include "Core/Editor.hpp"
#include "System/ISystem.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Core/PlatformProcess.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{
	GUINodeTextArea::GUINodeTextArea(GUIDrawerBase* drawer, int drawOrder) : GUINodeScrollArea(drawer, drawOrder)
	{
		m_input = m_drawer->GetEditor()->GetSystem()->CastSubsystem<Input>(SubsystemType::Input);
		SetDirection(Direction::Horizontal);
		m_minY = GetStoreSize("A"_hs, "A").y;
	}

	GUINodeTextArea::~GUINodeTextArea()
	{
		m_editor->GetCommandManager()->OnReferenceDestroyed(this);
	}

	void GUINodeTextArea::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		const float padding		 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightTall, m_window->GetDPIScale());
		Vector2		lastTextSize = GetStoreSize("TitleText"_hs, m_title);
		lastTextSize.y			 = Math::Max(lastTextSize.y, m_minY);
		m_caretHeight			 = widgetHeight * 0.6f;

		// Adjust rect
		{
			SetWidgetHeight(ThemeProperty::WidgetHeightTall);
			if (!m_widthOverridenOutside)
				GUINode::ConsumeAvailableWidth();
		}

		// Bg
		{
			DrawBackground(threadID);
		}

		GUIUtility::SetClip(threadID, m_rect, Rect(Vector2(padding + m_textOffset, 0), Vector2(-padding * 2.0f - m_textOffset, 0)));

		// Text
		{
			LinaVG::TextOptions opts;
			opts.font		 = Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
			opts.color		 = LV4((!m_disabled ? Color::White : Theme::TC_Silent2));
			m_initialTextPos = Vector2(m_rect.pos.x + padding - m_scrollValue + m_textOffset, m_rect.pos.y + m_rect.size.y * 0.5f + lastTextSize.y * 0.5f);
			LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(m_initialTextPos), opts, 0.0f, m_drawOrder, true);
		}

		// Caret - multiselection background
		{
			DrawCaretAndSelection(threadID);
		}

		// Mouse curosr
		{
			HandleMouseCursor();
		}

		GUIUtility::UnsetClip(threadID);
	}

	void GUINodeTextArea::OnPressBegin(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		if (m_characters.empty())
		{
			StartEditing();
			m_caretIndexEnd = m_caretIndexStart = 0;
			return;
		}

		StartEditing();

		uint32 caretIndex = FindCaretIndexFromMouse();
		m_caretIndexEnd = m_caretIndexStart = caretIndex;
	}

	void GUINodeTextArea::OnLostFocus()
	{
		if (m_isEditing)
			FinishEditing();
	}

	void GUINodeTextArea::SetTitle(const String& title)
	{
		GUINode::SetTitle(title);
		m_characters.clear();

		float		   x	 = 0.0f;
		Vector<String> chars = Internal::SplitIntoUTF8Chars(title.c_str());

		for (const String& ch : chars)
		{
			const char*	   c_str	= ch.c_str();
			const StringID sid		= TO_SIDC(c_str);
			const Vector2  charSize = GetStoreSize(sid, c_str, FontType::DefaultEditor);
			const CharData cd		= CharData{.x = x, .sizeX = charSize.x, .byteCount = static_cast<uint8>(ch.size())};
			m_characters.push_back(cd);
			x += charSize.x;
		}
	}

	void GUINodeTextArea::OnDoubleClicked()
	{
		SelectAll();
	}

	void GUINodeTextArea::OnKey(uint32 key, InputAction act)
	{
		if (!m_hasFocus || act == InputAction::Released)
			return;

		if (key == LINA_KEY_RETURN)
		{
			if (m_isEditing)
				FinishEditing();
			else
				SelectAll();
		}

		if (!m_isEditing)
			return;

		const uint32 sz = static_cast<uint32>(m_characters.size());

		if (key == LINA_KEY_RIGHT)
		{
			if (m_caretIndexEnd != m_caretIndexStart)
			{
				if (m_caretIndexEnd != sz)
					m_caretIndexEnd++;
			}
			else
			{
				if (m_caretIndexStart != sz)
					m_caretIndexStart = ++m_caretIndexEnd;
			}

			CheckCaretIndexAndScroll(false);
		}
		else if (key == LINA_KEY_LEFT)
		{
			if (m_caretIndexEnd != m_caretIndexStart)
			{
				if (m_caretIndexEnd != 0)
					m_caretIndexEnd--;
			}
			else
			{
				if (m_caretIndexStart != 0)
					m_caretIndexStart = --m_caretIndexEnd;
			}

			CheckCaretIndexAndScroll(true);
		}
		else if (key == LINA_KEY_DOWN || key == LINA_KEY_UP)
		{
			m_caretIndexStart = m_caretIndexEnd;
			CheckCaretIndexAndScroll(false);
		}
		else
		{
			if (key == LINA_KEY_BACKSPACE)
			{
				EraseChar();
			}
			else if (m_input->GetCharacterMask(key) & CharacterMask::Printable)
			{
				if (m_input->IsControlPressed())
					return;

				wchar_t ch = m_input->GetCharacterFromKey(key);

				if (ch != 0)
				{
					if (m_caretIndexEnd != m_caretIndexStart)
						EraseChar();

					const String encoded = Internal::EncodeUTF8(ch);
					String		 copy	 = m_title;

					if (m_caretIndexEnd == sz)
					{
						copy.append(encoded);
						m_caretIndexStart = ++m_caretIndexEnd;
					}
					else
					{
						uint32 pos = 0;
						for (uint32 i = 0; i < m_caretIndexEnd; i++)
							pos += m_characters[i].byteCount;

						m_caretIndexStart = ++m_caretIndexEnd;
						copy.insert(pos, encoded);
					}

					SetTitle(copy);
					CheckCaretIndexAndScroll(false);
				}
			}
		}
	}

	bool GUINodeTextArea::OnShortcut(Shortcut sc)
	{
		if (!m_isEditing)
			return false;

		if (sc == Shortcut::CTRL_C || sc == Shortcut::CTRL_X)
		{
			if (m_caretIndexEnd != m_caretIndexStart)
			{
				const uint32 min	= Math::Min(m_caretIndexStart, m_caretIndexEnd);
				const uint32 max	= Math::Max(m_caretIndexStart, m_caretIndexEnd);
				uint32		 posMin = 0, posMax = 0;
				FindPositions(min, max, posMin, posMax);

				const String  substr = m_title.substr(posMin, posMax - posMin);
				const WString wstr	 = Internal::StringToWString(substr);
				PlatformProcess::CopyToClipboard(wstr.c_str());

				if (sc == Shortcut::CTRL_X)
					EraseChar();

				CheckCaretIndexAndScroll(false);
			}
		}
		else if (sc == Shortcut::CTRL_A)
		{
			SelectAll();
		}
		else if (sc == Shortcut::CTRL_V)
		{
			WString wstr = L"";
			if (PlatformProcess::TryGetStringFromClipboard(wstr))
			{
				if (m_caretIndexEnd != m_caretIndexStart)
					EraseChar();

				const String str = Internal::WideStringToString(wstr);

				uint32 posMin = 0, posMax = 0;
				FindPositions(m_caretIndexStart, m_caretIndexEnd, posMin, posMax);

				String copy = m_title;
				copy.insert(posMax, str);

				const uint32 szNow = static_cast<uint32>(m_characters.size());
				SetTitle(copy);

				const uint32 szDiff = static_cast<uint32>(m_characters.size()) - szNow;
				m_caretIndexEnd += szDiff;
				m_caretIndexStart = m_caretIndexEnd;

				CheckCaretIndexAndScroll(false);
			}
		}

		return true;
	}

	void GUINodeTextArea::OnChildExceededSize(float amt)
	{
		// AddScrollValue(amt);
	}

	void GUINodeTextArea::OnHoverEnd()
	{
		m_window->SetCursorType(CursorType::Default);
	}

	void GUINodeTextArea::DrawCaretAndSelection(int threadID)
	{
		if (!m_isEditing)
			return;

		LinaVG::StyleOptions caretStyle;
		caretStyle.color		 = LV4(Theme::TC_Silent3);
		caretStyle.color.start.w = caretStyle.color.end.w = 0.5f;
		caretStyle.thickness							  = 1.2f * m_window->GetDPIScale();

		// Caret
		{
			// First timer
			{
				m_caretTimer += SystemInfo::GetDeltaTimeF();
				if (m_caretTimer > 0.5f)
				{
					m_caretTimer	  = 0.0f;
					m_shouldDrawCaret = !m_shouldDrawCaret;
				}
			}

			if (m_isPressed && !m_characters.empty())
			{
				const uint32 current = m_caretIndexEnd;
				m_caretIndexEnd		 = FindCaretIndexFromMouse();

				if (current != m_caretIndexEnd)
				{
					CheckCaretIndexAndScroll(m_caretIndexEnd < m_caretIndexStart);
				}
			}

			if (m_shouldDrawCaret)
			{
				const uint32 caretIndex = m_caretIndexEnd;
				float		 caretX		= 0.0f;
				const uint32 sz			= static_cast<uint32>(m_characters.size());

				if (sz != 0)
				{
					if (caretIndex == sz)
					{
						const auto& cd = m_characters[sz - 1];
						caretX		   = cd.x + cd.sizeX;
					}
					else
						caretX = m_characters[caretIndex].x;
				}

				const Vector2 p1 = Vector2(m_initialTextPos.x + caretX, m_rect.pos.y + m_rect.size.y * 0.5f - m_caretHeight * 0.5f);
				const Vector2 p2 = Vector2(m_initialTextPos.x + caretX, m_rect.pos.y + m_rect.size.y * 0.5f + m_caretHeight * 0.5f);
				LinaVG::DrawLine(threadID, LV2(p1), LV2(p2), caretStyle, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder + 1);
			}
		}

		// Background if multi-selected
		if (m_caretIndexStart != m_caretIndexEnd)
		{
			LinaVG::StyleOptions opts;
			opts.color		   = LV4(Theme::TC_CyanAccent);
			opts.color.start.w = opts.color.end.w = 0.2f;

			const uint32 sz = static_cast<uint32>(m_characters.size());

			const uint32 from = m_caretIndexStart;
			const uint32 to	  = m_caretIndexEnd;

			if (from < to)
			{
				const float	  startX = m_characters[from].x;
				const float	  endX	 = to == sz ? (m_characters[sz - 1].x + m_characters[sz - 1].sizeX) : m_characters[to].x;
				const Vector2 start	 = Vector2(m_initialTextPos.x + startX, m_rect.pos.y + m_rect.size.y * 0.5f - m_caretHeight * 0.5f);
				const Vector2 end	 = Vector2(m_initialTextPos.x + endX, start.y + m_caretHeight);
				LinaVG::DrawRect(threadID, LV2(start), LV2(end), opts, 0.0f, m_drawOrder);
			}
			else if (from > to)
			{
				const float	  startX = m_characters[to].x;
				const float	  endX	 = from == sz ? (m_characters[sz - 1].x + m_characters[sz - 1].sizeX) : m_characters[from].x;
				const Vector2 start	 = Vector2(m_initialTextPos.x + startX, m_rect.pos.y + m_rect.size.y * 0.5f - m_caretHeight * 0.5f);
				const Vector2 end	 = Vector2(m_initialTextPos.x + endX, start.y + m_caretHeight);
				LinaVG::DrawRect(threadID, LV2(start), LV2(end), opts, 0.0f, m_drawOrder);
			}
		}
	}

	uint32 GUINodeTextArea::FindCaretIndexFromMouse()
	{
		uint32		  caretIndex	 = 0;
		const Vector2 mp			 = m_window->GetMousePosition();
		const auto&	  lastChar		 = m_characters[m_characters.size() - 1];
		const float	  xAlpha		 = mp.x - m_rect.pos.x;
		const float	  textStartAlpha = m_initialTextPos.x - m_rect.pos.x;
		const float	  textEndAlpha	 = m_initialTextPos.x + lastChar.x + lastChar.sizeX;

		const uint32 sz = static_cast<uint32>(m_characters.size());

		if (xAlpha < textStartAlpha)
			caretIndex = 0;
		else if (xAlpha > m_textOffset + textEndAlpha - textStartAlpha)
			caretIndex = sz;
		else
		{
			for (uint32 i = 0; i < sz; i++)
			{
				const auto& cd = m_characters[i];

				float compare = 0.0f;

				if (i == 0)
				{
					compare = cd.x;
				}
				else
				{
					const auto& prev = m_characters[i - 1];
					compare			 = prev.x + prev.sizeX * 0.5f;
				}

				if (xAlpha - textStartAlpha > compare)
					caretIndex = i;
			}
		}

		return caretIndex;
	}

	void GUINodeTextArea::StartEditing()
	{
		if (m_disabled)
			return;

		m_isEditing	   = true;
		m_preEditTitle = m_title;
	}

	void GUINodeTextArea::FinishEditing()
	{
		m_isEditing		  = false;
		m_caretIndexStart = 0;
		m_caretIndexEnd	  = 0;

		if (m_preEditTitle.compare(m_title) == 0)
			return;

		if (!VerifyTitle())
		{
			SetTitle(m_preEditTitle);
			m_preEditTitle.clear();
		}
		else
		{
			m_editor->GetCommandManager()->AddCommand(new EditorCommandTextEdit(m_editor, this, m_preEditTitle, m_title));
			OnTitleChanged(m_title);
		}
	}

	void GUINodeTextArea::EraseChar()
	{
		// Debug title
		if (m_caretIndexEnd != m_caretIndexStart)
		{
			const uint32 min = Math::Min(m_caretIndexStart, m_caretIndexEnd);
			const uint32 max = Math::Max(m_caretIndexStart, m_caretIndexEnd);

			uint32 posMin = 0, posMax = 0;
			FindPositions(min, max, posMin, posMax);

			String copy = m_title;
			copy.erase(posMin, posMax - posMin);
			m_characters.erase(m_characters.begin() + min, m_characters.begin() + max);
			m_caretIndexStart = m_caretIndexEnd = min;
			SetTitle(copy);
		}
		else
		{
			if (m_caretIndexEnd != 0)
			{
				const int32 indexToErase = m_caretIndexEnd - 1;
				const uint8 byteCount	 = m_characters[indexToErase].byteCount;

				uint32 position = 0;

				for (int32 i = 0; i < indexToErase; i++)
					position += m_characters[i].byteCount;

				String copy = m_title;
				copy.erase(position, byteCount);

				m_characters.erase(m_characters.begin() + m_caretIndexEnd - 1);
				m_caretIndexStart = --m_caretIndexEnd;
				SetTitle(copy);
			}
		}
		CheckCaretIndexAndScroll(false);
	}

	void GUINodeTextArea::SelectAll()
	{
		m_caretIndexStart = 0;
		m_caretIndexEnd	  = static_cast<uint32>(m_characters.size());
		StartEditing();
	}

	void GUINodeTextArea::FindPositions(uint32 min, uint32 max, uint32& posMin, uint32& posMax)
	{
		posMin = posMax = 0;
		for (uint32 i = 0; i < max; i++)
		{
			const auto& cd = m_characters[i];

			if (i < min)
				posMin += cd.byteCount;

			posMax += cd.byteCount;
		}
	}

	void GUINodeTextArea::CheckCaretIndexAndScroll(bool preferLeft)
	{
		if (m_characters.empty())
			return;

		const uint32 min		   = Math::Min(m_caretIndexStart, m_caretIndexEnd);
		const uint32 max		   = Math::Max(m_caretIndexStart, m_caretIndexEnd);
		const uint32 sz			   = static_cast<uint32>(m_characters.size());
		const float	 padding	   = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float	 visibleStartX = m_initialTextPos.x + m_scrollValue;
		float		 visibleEndX   = visibleStartX + m_rect.size.x - padding * 2.0f - m_textOffset;
		const float	 leftMargin	   = 8.0f;

		auto exec = [&](uint32 targetIndex) {
			float targetX = 0.0f;

			if (targetIndex == sz)
			{
				const auto& lastChar = m_characters[targetIndex - 1];
				targetX				 = visibleStartX + lastChar.sizeX + lastChar.x;
			}
			else
				targetX = visibleStartX + m_characters[targetIndex].x;

			if (targetX > visibleEndX + m_scrollValue)
				AddScrollValue(targetX - visibleEndX - m_scrollValue);
			else if (targetX < visibleStartX + m_scrollValue + leftMargin)
				AddScrollValue(-((visibleStartX + m_scrollValue + leftMargin) - targetX));
		};

		if (m_caretIndexEnd == m_caretIndexStart || !preferLeft)
			exec(max);
		else
			exec(min);
	}

	void GUINodeTextArea::DrawBackground(int threadID)
	{
		GUIUtility::DrawWidgetBackground(threadID, m_rect, m_window->GetDPIScale(), m_drawOrder, m_disabled, m_hasFocus);
	}

	bool GUINodeTextArea::VerifyTitle()
	{
		WString wstr = Internal::StringToWString(m_title);

		for (auto c : wstr)
		{
			uint32 keycode = m_input->GetKeycode(c);

			if (!m_inputMask.IsSet(m_input->GetCharacterMask(keycode)))
				return false;
		}

		return true;
	}

	void GUINodeTextArea::HandleMouseCursor()
	{
		if (GetIsHovered())
		{
			const float	  padding  = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
			const Vector2 start	   = Vector2(m_rect.pos.x + m_initialTextPos.x - padding, m_rect.pos.y);
			const Vector2 sz	   = Vector2(m_rect.size.x - m_initialTextPos.x, m_rect.size.y);
			const Rect	  textRect = Rect(start, sz);
			if (textRect.IsPointInside(m_window->GetMousePosition()))
			{
				m_window->SetCursorType(CursorType::Caret);
				return;
			}
		}
	}
} // namespace Lina::Editor
