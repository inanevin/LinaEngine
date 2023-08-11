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
#include "Platform/LinaVGIncl.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "GUI/Drawers/GUIDrawerBase.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "GUI/Nodes/Layouts/GUINodeScrollArea.hpp"
#include "GUI/Nodes/Widgets/GUINodeText.hpp"
#include "Core/Theme.hpp"
#include "Commands/EditorCommandWidgets.hpp"
#include "Math/Math.hpp"
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
		m_minY					   = GetStoreSize("A"_hs, "A").y;
		m_recalculateCharacterInfo = true;
		m_outTextData			   = new LinaVG::TextOutData();
	}

	GUINodeTextArea::~GUINodeTextArea()
	{
		m_editor->GetCommandManager()->OnReferenceDestroyed(this);
		delete m_outTextData;
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

		// Expand logic
		if (m_expandable)
		{
			const float	  triSize	 = m_window->GetDPIScale() * 14.0f;
			const float	  triYOffset = m_window->GetDPIScale() * 2;
			const Vector2 triStart	 = Vector2(m_rect.pos.x + m_rect.size.x - triSize, m_rect.pos.y + m_rect.size.y + triYOffset);
			const Vector2 triEnd	 = Vector2(triStart.x + triSize, triStart.y);
			const Vector2 triBot	 = Vector2(triStart.x + triSize * 0.5f, triStart.y + triSize * 0.6f);

			LinaVG::StyleOptions style;
			style.aaEnabled = true;
			style.color		= LV4(Theme::TC_Dark2);

			const Vector2 wp		   = m_window->GetPos();
			const Rect	  triangleRect = Rect(wp + triStart, Vector2(triEnd.x - triStart.x, triBot.y - triStart.y));
			const Vector2 mp		   = m_input->GetMousePositionAbs();
			const bool	  hovered	   = triangleRect.IsPointInside(mp);

			if (hovered || m_draggingExpandRect)
				style.color = LV4(Theme::TC_Light1);

			LinaVG::DrawTriangle(threadID, LV2(triStart), LV2(triEnd), LV2(triBot), style, 0.0f, m_drawOrder);

			if (hovered && m_input->GetMouseButtonDown(LINA_MOUSE_0))
			{
				m_draggingExpandRect = true;
			}

			if (m_draggingExpandRect)
			{
				m_scrollValue = 0.0f;
				m_additionalHeight += m_window->GetMouseDelta().y;
				m_additionalHeight = Math::Clamp(m_additionalHeight, 0.0f, widgetHeight * 6);

				if (!m_input->GetMouseButton(LINA_MOUSE_0))
					m_draggingExpandRect = false;
			}
		}

		m_expandActive = m_expandable && !Math::Equals(m_additionalHeight, 0.0f, 0.001f);
		m_direction	   = m_expandActive ? Direction::Vertical : Direction::Horizontal;

		// Adjust rect
		{
			m_rect.size.y = widgetHeight + m_additionalHeight;

			if (!m_widthOverridenOutside)
				GUINode::ConsumeAvailableWidth();
		}

		bool shouldRecalcCharInfo = false;

		if (m_isEditing)
			m_recalculateCharacterInfo = true;

		{
			if (m_recalculateCharacterInfo || !Math::Equals(m_lastWidth, m_rect.size.x, 0.001f))
			{
				shouldRecalcCharInfo	   = true;
				m_recalculateCharacterInfo = false;
				m_outTextData->Shrink();
			}

			m_lastWidth = m_rect.size.x;
		}

		// Bg
		{
			DrawBackground(threadID);
		}

		if (m_expandActive)
			GUIUtility::SetClip(threadID, m_rect, Rect(Vector2(padding + m_textOffset, padding * 0.2f), Vector2(-padding * 2.0f - m_textOffset, -padding * 0.2f * 2.0f)));
		else
			GUIUtility::SetClip(threadID, m_rect, Rect(Vector2(padding + m_textOffset, 0), Vector2(-padding * 2.0f - m_textOffset, 0)));

		// Text
		{
			LinaVG::TextOptions opts;
			opts.font  = Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
			opts.color = LV4((!m_disabled ? Color::White : Theme::TC_Silent2));

			float posY = 0.0f;

			if (m_expandActive)
			{
				posY			 = m_rect.pos.y + widgetHeight * 0.5f + m_minY * 0.5f;
				m_wrapWidth		 = m_rect.size.x - padding * 2.0f;
				opts.wrapWidth	 = m_wrapWidth;
				m_initialTextPos = Vector2(m_rect.pos.x + padding + m_textOffset, posY - m_scrollValue);
			}
			else
			{
				posY			 = m_rect.pos.y + m_rect.size.y * 0.5f + lastTextSize.y * 0.5f;
				m_initialTextPos = Vector2(m_rect.pos.x + padding - m_scrollValue + m_textOffset, posY);
			}
			LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(m_initialTextPos), opts, 0.0f, m_drawOrder, shouldRecalcCharInfo, shouldRecalcCharInfo ? m_outTextData : nullptr);
		}

		if (m_caretScrollCheckState != 0)
		{
			CheckCaretIndexAndScroll(m_caretScrollCheckState == 1 ? false : true);
			m_caretScrollCheckState = 0;
		}

		// Caret - multiselection background
		{
			DrawCaretAndSelection(threadID);
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

		SetCaretFromMouse(true);
		m_caretScrollCheckState = 1;
		m_caretIndexEnd			= m_caretIndexStart;
	}

	void GUINodeTextArea::OnLostFocus()
	{
		if (m_isEditing)
			FinishEditing();
	}

	void GUINodeTextArea::SetTitle(const String& title)
	{
		if (title.compare(m_title) == 0)
			return;

		GUINode::SetTitle(title);
		m_characters.clear();
		m_recalculateCharacterInfo = true;

		float		   x	 = 0.0f;
		Vector<String> chars = Internal::SplitIntoUTF8Chars(title.c_str());

		for (const String& ch : chars)
		{
			const char*	   c_str	= ch.c_str();
			const StringID sid		= TO_SIDC(c_str);
			const Vector2  charSize = GetStoreSize(sid, c_str, FontType::DefaultEditor);
			const CharData cd		= CharData{.byteCount = static_cast<uint8>(ch.size())};
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

			m_caretScrollCheckState = 1;
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

			m_caretScrollCheckState = 2;
		}
		else if (key == LINA_KEY_DOWN || key == LINA_KEY_UP)
		{
			if (m_expandActive)
			{
				const auto& currentLine = m_outTextData->lineInfo[m_caretLineEnd];

				if (key == LINA_KEY_UP)
				{
					if (m_caretLineEnd != 0)
					{
						const auto& targetLine = m_outTextData->lineInfo[--m_caretLineEnd];
						m_caretIndexStart = m_caretIndexEnd = GetIndexInLine(m_caretLineEnd, m_outTextData->characterInfo[m_caretIndexEnd].x);
						m_caretLineStart					= m_caretLineEnd;
						m_caretScrollCheckState				= 1;
					}
				}
				else if (key == LINA_KEY_DOWN)
				{
					if (m_caretLineEnd != static_cast<uint32>(m_outTextData->lineInfo.m_size) - 1)
					{
						const auto& targetLine = m_outTextData->lineInfo[++m_caretLineEnd];
						m_caretIndexStart = m_caretIndexEnd = GetIndexInLine(m_caretLineEnd, m_outTextData->characterInfo[m_caretIndexEnd].x);
						m_caretLineStart					= m_caretLineEnd;
						m_caretScrollCheckState				= 1;
					}
				}
			}
			else
			{
				m_caretIndexStart		= m_caretIndexEnd;
				m_caretScrollCheckState = 1;
			}
		}
		else
		{
			if (key == LINA_KEY_BACKSPACE)
			{
				EraseChar();
			}
			else if (m_input->GetCharacterMask(m_input->GetCharacterFromKey(key)) & CharacterMask::Printable)
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
					m_caretScrollCheckState = 1;
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

				m_caretScrollCheckState = 1;
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

				// prefer right
				m_caretScrollCheckState = 1;
			}
		}

		return true;
	}

	void GUINodeTextArea::OnChildExceededSize(float amt)
	{
		// AddScrollValue(amt);
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
				const uint32   current = m_caretIndexEnd;
				const Vector2i delta   = m_window->GetMouseDelta();

				if (!m_expandActive || delta.x != 0 || delta.y != 0)
					SetCaretFromMouse(false);

				if (current != m_caretIndexEnd)
				{
					m_caretScrollCheckState = m_caretIndexEnd < m_caretIndexStart ? 1 : 2;
				}
			}

			if (m_shouldDrawCaret)
			{
				float		 caretX		= 0.0f;
				const uint32 sz			= static_cast<uint32>(m_outTextData->characterInfo.m_size);
				const uint32 caretIndex = m_caretIndexEnd;

				float targetX = 0.0f;
				float targetY = 0.0f;

				if (sz == 0)
				{
					targetX = m_initialTextPos.x;
					targetY = m_initialTextPos.y;
				}
				else if (caretIndex == sz)
				{
					const auto& last = m_outTextData->characterInfo[caretIndex - 1];
					targetX			 = last.x + last.sizeX;
					targetY			 = last.y;
				}
				else
				{
					const auto& tgt = m_outTextData->characterInfo[caretIndex];
					targetX			= tgt.x;
					targetY			= tgt.y;
				}

				const Vector2 p1 = Vector2(targetX, targetY - m_caretHeight);
				const Vector2 p2 = Vector2(targetX, targetY + m_caretHeight * 0.1f);
				LinaVG::DrawLine(threadID, LV2(p1), LV2(p2), caretStyle, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder + 1);
			}
		}

		// Background if multi-selected
		if (m_caretIndexStart != m_caretIndexEnd)
		{
			LinaVG::StyleOptions opts;
			opts.color		   = LV4(Theme::TC_CyanAccent);
			opts.color.start.w = opts.color.end.w = 0.2f;
			const uint32 sz						  = static_cast<uint32>(m_outTextData->characterInfo.m_size);

			const float	 lineHeight		 = m_minY * 1.2f;
			const float	 lineExtraBottom = m_minY * 0.35f;
			const uint32 min			 = Math::Min(m_caretIndexStart, m_caretIndexEnd);
			const uint32 max			 = Math::Max(m_caretIndexStart, m_caretIndexEnd);

			const uint32 lineMin = Math::Min(m_caretLineStart, m_caretLineEnd);
			const uint32 lineMax = Math::Max(m_caretLineStart, m_caretLineEnd);

			if (m_outTextData->lineInfo.isEmpty() || m_caretLineStart == m_caretLineEnd)
			{

				const auto& minChar = m_outTextData->characterInfo[min];
				const auto& maxChar = m_outTextData->characterInfo[max == sz ? sz - 1 : max];

				const Vector2 start = Vector2(minChar.x, minChar.y - lineHeight);
				const Vector2 end	= Vector2(maxChar.x + maxChar.sizeX, minChar.y + lineExtraBottom);

				LinaVG::DrawRect(threadID, LV2(start), LV2(end), opts, 0.0f, m_drawOrder);
			}
			else
			{
				const auto& firstCharacter		   = m_outTextData->characterInfo[min];
				const auto& lastCharacter		   = m_outTextData->characterInfo[max == sz ? max - 1 : max];
				const auto& firstLineLastCharacter = m_outTextData->characterInfo[m_outTextData->lineInfo[lineMin].endCharacterIndex];
				const auto& lastLineFirstCharacter = m_outTextData->characterInfo[m_outTextData->lineInfo[lineMax].startCharacterIndex];

				// First line coverage
				const Vector2 firstLineStart = Vector2(firstCharacter.x, firstCharacter.y - lineHeight);
				const Vector2 firstLineEnd	 = Vector2(firstLineLastCharacter.x, firstCharacter.y + lineExtraBottom);
				LinaVG::DrawRect(threadID, LV2(firstLineStart), LV2(firstLineEnd), opts, 0.0f, m_drawOrder);

				// Last line coverage
				const Vector2 lastLineStart = Vector2(lastLineFirstCharacter.x, lastLineFirstCharacter.y - lineHeight);
				const Vector2 lastLineEnd	= Vector2(lastCharacter.x + (max == sz ? lastCharacter.sizeX : 0.0f), lastLineFirstCharacter.y + lineExtraBottom);
				LinaVG::DrawRect(threadID, LV2(lastLineStart), LV2(lastLineEnd), opts, 0.0f, m_drawOrder);

				for (uint32 i = lineMin + 1; i < lineMax; i++)
				{
					const auto& midLineFirstCharacter = m_outTextData->characterInfo[m_outTextData->lineInfo[i].startCharacterIndex];
					const auto& midLineLastCharacter  = m_outTextData->characterInfo[m_outTextData->lineInfo[i].endCharacterIndex];

					const Vector2 midLineStart = Vector2(midLineFirstCharacter.x, midLineFirstCharacter.y - lineHeight);
					const Vector2 midLineEnd   = Vector2(midLineLastCharacter.x, midLineFirstCharacter.y + lineExtraBottom);
					LinaVG::DrawRect(threadID, LV2(midLineStart), LV2(midLineEnd), opts, 0.0f, m_drawOrder);
				}
			}
		}
	}

	void GUINodeTextArea::SetCaretFromMouse(bool isStartIndex)
	{
		const Vector2 mp		 = m_window->GetMousePosition();
		const Vector2 mouseLocal = mp - Vector2(m_initialTextPos.x, m_rect.pos.y);

		uint32 chosenLine = 0;
		int32  chosen	  = 0;

		const int32 sz = static_cast<int32>(m_outTextData->characterInfo.m_size);

		auto findCharacter = [&](int32 beginIndex, int32 endIndex) {
			float minHorizontalDistance = 9999.0f;
			int32 selected				= -1;
			for (int32 i = beginIndex; i < endIndex; i++)
			{
				auto&		  charInfo			 = m_outTextData->characterInfo[i];
				const Vector2 localPos			 = Vector2(charInfo.x - m_initialTextPos.x, charInfo.y - m_rect.pos.y);
				float		  horizontalDistance = std::abs(mouseLocal.x - localPos.x);

				if (horizontalDistance < minHorizontalDistance)
				{
					minHorizontalDistance = horizontalDistance;
					selected			  = i;
				}
			}

			return selected;
		};

		if (m_outTextData->lineInfo.m_size == 0)
		{
			// single line
			chosen = findCharacter(0, sz);

			if (chosen == sz - 1 && mouseLocal.x > (m_outTextData->characterInfo[sz - 1].x - m_initialTextPos.x))
				chosen = sz;
		}
		else
		{
			// find which line
			uint32		lastLine = static_cast<uint32>(m_outTextData->lineInfo.m_size) - 1;
			const auto& fl		 = m_outTextData->lineInfo[0];
			const auto& ll		 = m_outTextData->lineInfo[lastLine];
			if (mp.y < fl.posY || Math::Equals(mp.y, fl.posY, 0.1f))
				chosenLine = 0;
			else if (mp.y > ll.posY || Math::Equals(mp.y, ll.posY, 0.1f))
				chosenLine = lastLine;
			else
			{
				for (uint32 i = lastLine; i > 0; i--)
				{
					if (mp.y < m_outTextData->lineInfo[i].posY)
						chosenLine = i;
				}
			}

			const auto& selectedLine = m_outTextData->lineInfo[chosenLine];
			chosen					 = findCharacter(selectedLine.startCharacterIndex, selectedLine.endCharacterIndex + 1);

			if (chosenLine == lastLine && chosen == selectedLine.endCharacterIndex && mouseLocal.x > (m_outTextData->characterInfo[selectedLine.endCharacterIndex].x - m_initialTextPos.x))
				chosen = selectedLine.endCharacterIndex + 1;
		}

		if (isStartIndex)
		{
			m_caretIndexStart = static_cast<uint32>(chosen);
			m_caretLineStart  = chosenLine;
		}
		else
		{
			m_caretIndexEnd = static_cast<uint32>(chosen);
			m_caretLineEnd	= chosenLine;
		}
	}

	void GUINodeTextArea::StartEditing()
	{
		if (m_disabled || !m_allowEditing)
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

		bool		 titleOK = false;
		const String ret	 = VerifyTitle(titleOK);

		if (!titleOK)
		{
			SetTitle(m_preEditTitle);
			m_preEditTitle.clear();
		}
		else
		{
			m_editor->GetCommandManager()->AddCommand(new EditorCommandTextEdit(m_editor, this, m_preEditTitle, ret));
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
		m_caretScrollCheckState = 1;
	}

	void GUINodeTextArea::SelectAll()
	{
		m_caretIndexStart = 0;
		m_caretIndexEnd	  = static_cast<uint32>(m_characters.size());
		m_caretLineStart  = 0;
		m_caretLineEnd	  = m_outTextData->lineInfo.m_size - 1;
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

		const float	 padding	  = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float	 widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightTall, m_window->GetDPIScale());
		const uint32 min		  = Math::Min(m_caretIndexStart, m_caretIndexEnd);
		const uint32 max		  = Math::Max(m_caretIndexStart, m_caretIndexEnd);
		const uint32 sz			  = static_cast<uint32>(m_outTextData->characterInfo.m_size);

		if (m_expandActive)
		{
			m_caretLineStart = FindLineFromIndex(m_caretIndexStart);

			if (m_caretIndexStart == m_caretIndexEnd)
				m_caretLineEnd = m_caretLineStart;
			else
				m_caretLineEnd = FindLineFromIndex(m_caretIndexEnd);

			const auto& maxChar	  = m_outTextData->characterInfo[max == sz ? max - 1 : max];
			const auto& caretLine = m_outTextData->lineInfo[m_caretLineEnd];
			const float localY	  = caretLine.posY - m_rect.pos.y - m_scrollValue;
			const float minY	  = m_rect.pos.y + widgetHeight * 0.5f + m_minY * 0.5f;
			const float maxY	  = m_rect.pos.y + m_rect.size.y - widgetHeight * 0.5f - m_minY * 0.5f;
			const float desiredY  = Math::Clamp(caretLine.posY, Math::Min(minY, maxY), Math::Max(minY, maxY));

			if (caretLine.posY > desiredY || caretLine.posY < desiredY)
				AddScrollValue(caretLine.posY - desiredY);
		}
		else
		{
			const auto& targetChar = m_outTextData->characterInfo[!preferLeft ? (min == sz ? sz - 1 : min) : (max == sz ? sz - 1 : max)];
			const float minX	   = m_rect.pos.x + padding;
			const float maxX	   = m_rect.pos.x + m_rect.size.x - padding * 1.5f;
			const float desiredX   = Math::Clamp(targetChar.x, minX, maxX);

			if (targetChar.x > desiredX || targetChar.x < desiredX)
				AddScrollValue(targetChar.x - desiredX);
		}
	}

	uint32 GUINodeTextArea::FindLineFromIndex(uint32 index)
	{
		if (index == m_outTextData->characterInfo.m_size)
			index--;

		const uint32 sz = static_cast<uint32>(m_outTextData->lineInfo.m_size);
		for (uint32 i = 0; i < sz; i++)
		{
			const auto& line = m_outTextData->lineInfo[i];
			if (index >= line.startCharacterIndex && index <= line.endCharacterIndex)
				return i;
		}

		return 0;
	}

	uint32 GUINodeTextArea::GetIndexInLine(uint32 lineIndex, float posX)
	{

		const auto& line  = m_outTextData->lineInfo[lineIndex];
		uint32		index = line.endCharacterIndex;

		float minDistance = 999.0f;
		for (uint32 i = line.startCharacterIndex; i < line.endCharacterIndex + 1; i++)
		{
			const float charPosX = m_outTextData->characterInfo[i].x;
			const float dist	 = Math::Abs(charPosX - posX);
			if (dist < minDistance)
			{
				minDistance = dist;
				index		= i;
			}
		}

		return index;
	}

	void GUINodeTextArea::DrawBackground(int threadID)
	{
		GUIUtility::DrawWidgetBackground(threadID, m_rect, m_window->GetDPIScale(), m_drawOrder, m_disabled, m_hasFocus);
	}

	String GUINodeTextArea::VerifyTitle(bool& titleOK)
	{
		WString wstr = Internal::StringToWString(m_title);

		for (auto c : wstr)
		{
			if (!m_inputMask.IsSet(m_input->GetCharacterMask(c)))
			{
				titleOK = false;
				return "";
			}
		}

		titleOK = true;
		return m_title;
	}

} // namespace Lina::Editor
