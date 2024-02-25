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

#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Common/Math/Math.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

#define CARET_ALPHA_SPEED 8.0f

	void InputField::Construct()
	{
		m_text						 = Allocate<Text>("InputFieldText");
		m_text->GetProps().isDynamic = true;
		AddChild(m_text);
	}

	void InputField::Tick(float delta)
	{
		const bool prevHovered = m_isHovered;
		Widget::SetIsHovered();

		const bool hasControls = m_manager->GetControlsOwner() == this;

		if (!hasControls)
			m_isEditing = false;

		// Cursor status.
		if (m_isHovered)
		{
			if (m_props.isNumberField && !m_props.disableNumberSlider)
			{
				if (m_isEditing)
					m_lgxWindow->SetCursorType(LinaGX::CursorType::Caret);
			}
			else
				m_lgxWindow->SetCursorType(LinaGX::CursorType::Caret);
		}
		else
		{
			if (prevHovered)
				m_lgxWindow->SetCursorType(LinaGX::CursorType::Default);
		}

		// Number field slider movement.
		if (m_middlePressed && !m_isEditing && m_props.isNumberField && m_props.value)
		{
			const Vector2 mouse		= m_lgxWindow->GetMousePosition();
			const float	  perc		= Math::Remap(mouse.x, m_rect.pos.x, m_rect.pos.x + m_rect.size.x, 0.0f, 1.0f);
			float		  targetVal = Math::Remap(perc, 0.0f, 1.0f, m_props.valueMin, m_props.valueMax);

			if (!Math::IsZero(m_props.valueStep))
			{
				const float prev = *m_props.value;
				const float diff = targetVal - prev;
				*m_props.value	 = prev + m_props.valueStep * Math::FloorToFloat(diff / m_props.valueStep);
			}
			else
				*m_props.value = targetVal;

			if (m_props.clampNumber)
				*m_props.value = Math::Clamp(*m_props.value, m_props.valueMin, m_props.valueMax);

			if (m_props.onValueChanged)
				m_props.onValueChanged(*m_props.value);
		}

		if (m_isPressed)
			m_caretInsertPos = GetCaretPosFromMouse();

		const Vector2& textSize		= m_text->GetSize();
		const Vector2  middle		= Vector2(m_rect.pos.x + m_props.horizontalIndent + textSize.x * 0.5f - m_textOffset, m_rect.pos.y + m_rect.size.y * 0.5f);
		m_textStartMid				= middle - Vector2(textSize.x * 0.5f, 0.0f);
		m_textEndMid				= middle + Vector2(textSize.x * 0.5f, 0.0f);
		const size_t characterCount = m_text->GetProps().text.size();
		m_averageCharacterStep		= characterCount == 0 ? 0.0f : textSize.x / static_cast<float>(characterCount);
		m_text->SetPos(middle - m_text->GetHalfSize());

		// Caret alpha
		if (m_isEditing)
		{
			m_caretCtr += CARET_ALPHA_SPEED * delta;
			if (m_caretCtr == 100.0f)
				m_caretCtr = 0.0f;
			m_caretAlpha = (Math::Sin(m_caretCtr) + 1.0f) / 2.0f;
		}
	}

	void InputField::Draw(int32 threadIndex)
	{
		const bool hasControls = m_manager->GetControlsOwner() == this;

		// Background
		LinaVG::StyleOptions style;
		style.color					   = m_props.colorBackground.AsLVG4();
		style.rounding				   = m_props.rounding;
		style.outlineOptions.thickness = m_props.outlineThickness;
		style.outlineOptions.color	   = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), (m_rect.pos + m_rect.size).AsLVG(), style, 0.0f, m_drawOrder);

		// Number field slider background.
		if (m_props.isNumberField && !m_props.disableNumberSlider && !m_isEditing && m_props.value)
		{
			const float			 perc = Math::Remap(*m_props.value, m_props.valueMin, m_props.valueMax, 0.0f, 1.0f);
			LinaVG::StyleOptions fill;
			fill.color.start		= m_props.colorNumberFillStart.AsLVG4();
			fill.color.end			= m_props.colorNumberFillEnd.AsLVG4();
			fill.color.gradientType = LinaVG::GradientType::Horizontal;
			fill.rounding			= m_props.rounding;

			const Vector2 start = m_rect.pos + Vector2(m_props.outlineThickness, m_props.outlineThickness);
			const Vector2 end	= m_rect.pos + m_rect.size - Vector2(m_props.outlineThickness, m_props.outlineThickness);
			const Vector2 sz	= end - start;

			LinaVG::DrawRect(threadIndex, start.AsLVG(), Vector2(start.x + sz.x * perc, end.y).AsLVG(), fill, 0.0f, m_drawOrder);
		}

		m_manager->SetClip(threadIndex, m_rect, {.left = m_props.horizontalIndent, .right = m_props.horizontalIndent});

		if (m_isEditing)
		{
			// Caret
			LinaVG::StyleOptions caret;
			m_props.colorCaret.w		   = m_caretAlpha;
			caret.color					   = m_props.colorCaret.AsLVG4();
			const Vector2 caretMiddle	   = GetPosFromCaretIndex(m_caretInsertPos);
			const Vector2 caretTopLeft	   = Vector2(caretMiddle.x - m_props.horizontalIndent * 0.1f, GetCaretStartY());
			const Vector2 caretBottomRight = Vector2(caretMiddle.x + m_props.horizontalIndent * 0.1f, GetCaretEndY());
			LinaVG::DrawRect(threadIndex, caretTopLeft.AsLVG(), caretBottomRight.AsLVG(), caret, 0, m_drawOrder);

			if (caretBottomRight.x > (m_rect.pos.x + m_rect.size.x - m_props.horizontalIndent))
			{
				const float diff = caretBottomRight.x - (m_rect.pos.x + m_rect.size.x - m_props.horizontalIndent);
				m_textOffset += m_averageCharacterStep;
			}
			if (caretTopLeft.x < (m_rect.pos.x + m_props.horizontalIndent))
			{
				m_textOffset -= m_averageCharacterStep;
				if (m_textOffset < 0.0f)
					m_textOffset = 0.0f;
			}

			// Highlight
			if (m_caretInsertPos != m_highlightStartPos)
			{
				const uint32		 minPos			   = Math::Min(m_highlightStartPos, m_caretInsertPos);
				const uint32		 maxPos			   = Math::Max(m_highlightStartPos, m_caretInsertPos);
				const Vector2		 highlightStartMid = GetPosFromCaretIndex(minPos);
				const Vector2		 highlightEndMid   = GetPosFromCaretIndex(maxPos);
				const Vector2		 topLeft		   = Vector2(highlightStartMid.x, GetCaretStartY());
				const Vector2		 bottomRight	   = Vector2(highlightEndMid.x, GetCaretEndY());
				LinaVG::StyleOptions highlight;
				highlight.color = m_props.colorHighlight.AsLVG4();
				LinaVG::DrawRect(threadIndex, topLeft.AsLVG(), bottomRight.AsLVG(), highlight, 0, m_drawOrder);
			}
		}

		m_text->Draw(threadIndex);
		m_manager->UnsetClip(threadIndex);
	}

	void InputField::RenderSync()
	{
		// Assign text to number value.
		if (m_props.isNumberField && !m_isEditing && m_props.value)
		{
			const float value = *m_props.value;
			if (!Math::Equals(value, m_lastStoredValue, 0.0001f) || !m_syncedAtLeastOnce)
			{
				m_text->GetProps().text = UtilStr::FloatToString(value, m_props.decimals);
				m_text->CalculateTextSize();
				m_lastStoredValue	= value;
				m_syncedAtLeastOnce = true;
			}
		}
	}

	void InputField::SelectAll()
	{
		m_caretInsertPos	= static_cast<uint32>(m_text->GetProps().text.size());
		m_highlightStartPos = 0;
	}

	void InputField::EndEditing()
	{
		m_highlightStartPos = 0;
		m_caretInsertPos	= 0;
		m_isEditing			= false;
		m_textOffset		= 0.0f;

		if (m_props.onEditEnd)
			m_props.onEditEnd(m_text->GetProps().text);
	}

	Vector2 InputField::GetPosFromCaretIndex(uint32 index)
	{
		return Vector2(m_textStartMid.x + index * m_averageCharacterStep, m_rect.pos.y + m_rect.size.y * 0.5f);
	}

	uint32 InputField::GetCaretPosFromMouse()
	{
		const Vector2& mp  = m_lgxWindow->GetMousePosition();
		const float	   x   = mp.x - m_textStartMid.x;
		float		   off = Math::FloorToFloat(x / m_averageCharacterStep);
		if (off < 0.0f)
			off = 0.0f;
		const uint32 index = Math::Clamp((uint32)off, (uint32)0, static_cast<uint32>(m_text->GetProps().text.size()));
		return index;
	}

	bool InputField::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
	{
		if (!m_isEditing)
		{
			if (m_manager->GetControlsOwner() == this && keycode == LINAGX_KEY_RETURN && action != LinaGX::InputAction::Released)
			{
				m_isEditing = true;
				SelectAll();
				return true;
			}
			return false;
		}

		if (action == LinaGX::InputAction::Pressed || action == LinaGX::InputAction::Repeated)
		{
			const bool highlight = m_caretInsertPos != m_highlightStartPos;

			// Apply
			if (keycode == LINAGX_KEY_RETURN)
			{
				EndEditing();
				return true;
			}

			// Erase
			if (keycode == LINAGX_KEY_BACKSPACE)
			{
				RemoveCurrent();
				return true;
			}

			// Copy & paste
			if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LCTRL))
			{
				// Select all
				if (keycode == LINAGX_KEY_A)
				{
					SelectAll();
					return true;
				}

				// Copy / Cut
				if (keycode == LINAGX_KEY_C || keycode == LINAGX_KEY_X)
				{
					if (highlight)
					{
						const uint32 min	 = Math::Min(m_highlightStartPos, m_caretInsertPos);
						const uint32 max	 = Math::Max(m_highlightStartPos, m_caretInsertPos);
						const String copyStr = m_text->GetProps().text.substr(min, max - min);
						PlatformProcess::CopyToClipboard(copyStr.c_str());

						if (keycode == LINAGX_KEY_X)
							RemoveCurrent();
					}

					return true;
				}

				// Paste
				if (keycode == LINAGX_KEY_V)
				{
					if (highlight)
						RemoveCurrent();

					String str = "";
					PlatformProcess::TryGetStringFromClipboard(str);
					Insert(m_caretInsertPos, str);
					m_caretInsertPos += static_cast<size_t>(str.size());
					ClampCaretInsert();
					m_highlightStartPos = m_caretInsertPos;
					return true;
				}
			}

			// Apply characters
			uint16	mask	   = 0;
			wchar_t wcharacter = L' ';
			m_lgxWindow->GetInput()->GetCharacterInfoFromKeycode(keycode, wcharacter, mask);
			uint16 characterMask = LinaGX::CharacterMask::Number | LinaGX::CharacterMask::Separator | LinaGX::CharacterMask::Sign;
			if (!m_props.isNumberField)
				characterMask |= LinaGX::CharacterMask::Letter | LinaGX::CharacterMask::Symbol | LinaGX::CharacterMask::Whitespace;
			if (Bitmask<uint16>(mask).IsSet(characterMask))
			{
				if (highlight)
					RemoveCurrent();

				const String insert = UtilStr::WCharToString(wcharacter);
				const size_t sz		= insert.size();

				// We dont insert unicodes atm.
				// If we decide to do so, we need to increment caret position by unicode sz.
				// And when erasing, we should erase by sz.
				if (sz != 1)
					return true;

				Insert(m_caretInsertPos, insert);
				m_caretInsertPos++;
				ClampCaretInsert();
				m_highlightStartPos = m_caretInsertPos;
				return true;
			}

			// Move caret.
			if (keycode == LINAGX_KEY_LEFT && m_caretInsertPos > 0)
				m_caretInsertPos--;
			if (keycode == LINAGX_KEY_RIGHT)
				m_caretInsertPos++;
			ClampCaretInsert();
			if (!highlight)
				m_highlightStartPos = m_caretInsertPos;

			return true;
		}

		return false;
	}

	bool InputField::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		// Catch middle press
		if (m_props.isNumberField && button == LINAGX_MOUSE_MIDDLE && action == LinaGX::InputAction::Pressed && m_isHovered)
		{
			m_middlePressed = true;
			return true;
		}

		// Catch middle release
		if (m_props.isNumberField && button == LINAGX_MOUSE_MIDDLE && action == LinaGX::InputAction::Released && m_middlePressed)
		{
			m_middlePressed = false;
			return true;
		}

		if (button != LINAGX_MOUSE_0)
			return false;

		// Double clicks -> select all if editing.
		// Start editing if number slider.
		if (m_isHovered && action == LinaGX::InputAction::Repeated)
		{
			SelectAll();
			return true;
		}

		if (action == LinaGX::InputAction::Pressed && m_isHovered)
		{
			m_caretInsertPos	= GetCaretPosFromMouse();
			m_highlightStartPos = m_caretInsertPos;
			m_isPressed			= true;
			m_manager->GrabControls(this);
			m_isEditing = true;
			return true;
		}

		if (action == LinaGX::InputAction::Released && m_isPressed)
		{
			m_isPressed = false;
			return true;
		}

		return false;
	}

	float InputField::GetCaretStartY()
	{
		return m_rect.pos.y + m_rect.size.y * 0.1f;
	}

	float InputField::GetCaretEndY()
	{
		return m_rect.pos.y + m_rect.size.y - m_rect.size.y * 0.1f;
	}

	void InputField::Insert(uint32 pos, const String& str)
	{
		m_text->GetProps().text.insert(static_cast<size_t>(pos), str);
		m_text->CalculateTextSize();
		if (m_props.onEdited)
			m_props.onEdited(m_text->GetProps().text);

		if (m_props.isNumberField && m_props.value)
		{
			const String& str		   = m_text->GetProps().text;
			uint32		  outPrecision = 0;

			*m_props.value = UtilStr::StringToFloat(str, outPrecision);
			if (m_props.clampNumber)
				*m_props.value = Math::Clamp(*m_props.value, m_props.valueMin, m_props.valueMax);

			if (m_props.onValueChanged)
				m_props.onValueChanged(*m_props.value);
		}
	}

	void InputField::RemoveCurrent()
	{
		if (m_highlightStartPos == m_caretInsertPos)
		{
			if (m_caretInsertPos == 0)
				return;

			m_text->GetProps().text.erase(m_caretInsertPos - 1, 1);
			m_text->CalculateTextSize();
			m_caretInsertPos--;
			m_highlightStartPos = m_caretInsertPos;
		}
		else
		{
			const uint32 min   = Math::Min(m_caretInsertPos, m_highlightStartPos);
			const uint32 max   = Math::Max(m_caretInsertPos, m_highlightStartPos);
			const uint32 count = max - min;
			m_text->GetProps().text.erase(min, count);
			m_text->CalculateTextSize();
			m_caretInsertPos = m_highlightStartPos = min;
		}

		if (m_props.onEdited)
			m_props.onEdited(m_text->GetProps().text);

		if (m_text->GetProps().text.empty())
			m_textOffset = 0.0f;

		if (m_props.isNumberField && m_props.value)
		{
			const String& str		   = m_text->GetProps().text;
			uint32		  outPrecision = 0;

			*m_props.value = UtilStr::StringToFloat(str, outPrecision);
			if (m_props.clampNumber)
				*m_props.value = Math::Clamp(*m_props.value, m_props.valueMin, m_props.valueMax);

			if (m_props.onValueChanged)
				m_props.onValueChanged(*m_props.value);
		}
	}

	void InputField::ClampCaretInsert()
	{
		m_caretInsertPos	= Math::Clamp(m_caretInsertPos, (uint32)0, static_cast<uint32>(m_text->GetProps().text.size()));
		m_highlightStartPos = Math::Clamp(m_highlightStartPos, (uint32)0, static_cast<uint32>(m_text->GetProps().text.size()));
	}
} // namespace Lina
