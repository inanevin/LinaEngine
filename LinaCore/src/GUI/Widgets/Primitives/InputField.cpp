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
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

#define CARET_ALPHA_SPEED 8.0f

	void InputField::Construct()
	{
		GetWidgetProps().drawBackground = true;
		// GetWidgetProps().clipChildren			   = true;
		GetWidgetProps().childrenClipOffset.pos.x  = Theme::GetDef().baseIndent / 2;
		GetWidgetProps().childrenClipOffset.size.x = -Theme::GetDef().baseIndent;

		m_text										 = m_manager->Allocate<Text>("InputFieldText");
		m_text->GetProps().isDynamic				 = true;
		m_text->GetProps().fetchCustomClipFromParent = true;
		m_text->GetFlags().Set(WF_POS_ALIGN_Y);
		m_text->SetAlignedPosY(0.5f);
		m_text->GetWidgetProps().drawOrderIncrement = 1;
		m_text->SetAnchorY(Anchor::Center);

		m_placeholderText						= m_manager->Allocate<Text>("InputFieldPlaceholder");
		m_placeholderText->GetProps().isDynamic = true;
		m_placeholderText->GetFlags().Set(WF_POS_ALIGN_Y);
		m_placeholderText->SetAlignedPosY(0.5f);
		m_placeholderText->SetAnchorY(Anchor::Center);

		m_placeholderIcon = m_manager->Allocate<Icon>("InputFieldPlaceholderIcon");
		m_placeholderIcon->GetFlags().Set(WF_POS_ALIGN_Y);
		m_placeholderIcon->SetAlignedPosY(0.5f);
		m_placeholderIcon->SetAnchorY(Anchor::Center);

		AddChild(m_text);
		AddChild(m_placeholderText);
		AddChild(m_placeholderIcon);

		m_placeholderText->GetFlags().Set(WF_HIDE, true);
		m_placeholderIcon->GetFlags().Set(WF_HIDE, true);
	}

	void InputField::Initialize()
	{
		Widget::Initialize();

		if (m_props.valuePtr)
		{
			m_text->GetProps().text = UtilStr::FloatToString(GetValue(), m_props.decimals);
			m_text->CalculateTextSize();
		}

		if (m_props.valueStr)
		{
			m_text->GetProps().text = *m_props.valueStr;
			m_text->CalculateTextSize();
		}
	}

	void InputField::PreTick()
	{
		// Cursor status.
		if (m_isHovered && m_isEditing)
			m_manager->SetCursorOverride(static_cast<uint8>(LinaGX::CursorType::Caret));

		if (m_isEditing && m_manager->GetControlsOwner() != this)
			EndEditing(true);

		if (m_middlePressed && m_props.isNumberField && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_MIDDLE))
		{
			if (m_props.valuePtr && !Math::Equals(m_valueOnMiddlePress, GetValue(), 0.001f))
				PropagateCBOnEditEnded();
			m_middlePressed = false;
		}

		// Number field slider movement.
		if (m_middlePressed && !m_isEditing && m_props.isNumberField && m_props.valuePtr)
		{
			float		step = Math::IsZero(m_props.valueStep) == false ? m_props.valueStep : 1.0f;
			const float prev = GetValue();

			if (m_lgxWindow->GetInput()->GetKey(LINAGX_KEY_LSHIFT))
				step *= 0.005f;

			if (!m_props.disableNumberSlider)
			{
				const Vector2 mouse		= m_manager->GetMousePosition();
				const float	  perc		= Math::Clamp(Math::Remap(mouse.x, m_rect.pos.x, m_rect.pos.x + m_rect.size.x, 0.0f, 1.0f), 0.0f, 1.0f);
				float		  targetVal = Math::Remap(perc, 0.0f, 1.0f, m_props.valueMin, m_props.valueMax);
				const float	  diff		= targetVal - prev;
				SetValue(prev + step * Math::FloorToFloat(diff / step));
			}
			else
			{
				const Vector2 md = m_lgxWindow->GetMouseDelta();
				SetValue(prev + step * md.x);
			}

			if (m_props.clampNumber)
			{
				SetValue(Math::Clamp(GetValue(), m_props.valueMin, m_props.valueMax));
				m_lastStoredValue = INPF_VALUE_MIN;
			}

			PropagateCBOnEdited();
		}

		if (!m_isEditing && m_props.valueStr)
		{
			m_text->GetProps().text = *m_props.valueStr;
			m_text->CalculateTextSize();
		}
	}

	void InputField::Tick(float delta)
	{
		if (m_placeholderText->GetFlags().IsSet(WF_HIDE) && m_text->GetProps().text.empty())
		{
			m_placeholderText->GetProps().text	= m_props.placeHolderText;
			m_placeholderText->GetProps().color = m_props.colorPlaceHolder;
			m_placeholderText->CalculateTextSize();
			m_placeholderText->GetFlags().Remove(WF_HIDE);

			m_placeholderIcon->GetProps().icon	= m_props.placeHolderIcon;
			m_placeholderIcon->GetProps().color = m_props.colorPlaceHolder;
			m_placeholderIcon->CalculateIconSize();
			m_placeholderIcon->GetFlags().Remove(WF_HIDE);
		}
		else if (!m_placeholderText->GetFlags().IsSet(WF_HIDE) && !m_text->GetProps().text.empty())
		{
			m_placeholderText->GetFlags().Set(WF_HIDE);
			m_placeholderIcon->GetFlags().Set(WF_HIDE);
		}

		if (m_isPressed)
			m_caretInsertPos = GetCaretPosFromMouse();

		const Vector2& textSize = m_text->GetSize();
		const Vector2  middle	= Vector2(m_rect.pos.x + m_props.horizontalIndent + textSize.x * 0.5f - m_textOffset, m_rect.pos.y + m_rect.size.y * 0.5f);
		m_textStart				= middle - Vector2(textSize.x * 0.5f, 0.0f);
		m_textEnd				= middle + Vector2(textSize.x * 0.5f, 0.0f);

		if (m_props.centerText)
			m_textStart.x = m_rect.pos.x + GetHalfSizeX() - m_text->GetHalfSizeX();

		const size_t characterCount = m_text->GetProps().text.size();
		m_averageCharacterStep		= characterCount == 0 ? 0.0f : textSize.x / static_cast<float>(characterCount);
		m_text->SetPosX(m_textStart.x);

		if (m_props.placeHolderIcon.empty())
		{
			m_placeholderText->SetPosX(m_text->GetPosX());
		}
		else
		{
			m_placeholderIcon->SetPosX(m_text->GetPosX());
			m_placeholderText->SetPosX(m_placeholderIcon->GetSizeX() + m_text->GetPosX() + Theme::GetDef().baseIndentInner);
		}

		// Caret alpha
		if (m_isEditing)
		{
			m_caretCtr += CARET_ALPHA_SPEED * delta;
			if (m_caretCtr == 100.0f)
				m_caretCtr = 0.0f;
			m_caretAlpha = (Math::Sin(m_caretCtr) + 1.0f) / 2.0f;
		}

		// Assign text to number value.
		if (m_props.isNumberField && !m_isEditing && m_props.valuePtr)
		{
			const float value = GetValue();
			if (!Math::Equals(value, m_lastStoredValue, 0.0001f))
			{
				UpdateTextFromValue();
				m_lastStoredValue = value;
			}
		}
	}

	void InputField::Draw()
	{

		// Number field slider background.
		if (m_props.isNumberField && !m_props.disableNumberSlider && !m_isEditing && m_props.valuePtr)
		{
			const float			 perc = Math::Remap(GetValue(), m_props.valueMin, m_props.valueMax, 0.0f, 1.0f);
			LinaVG::StyleOptions fill;
			fill.color				= m_props.colorNumberFill.AsLVG();
			fill.color.gradientType = LinaVG::GradientType::Horizontal;
			fill.rounding			= m_widgetProps.rounding;

			const Vector2 start = m_rect.pos + Vector2(m_props.outlineThickness, m_props.outlineThickness);
			const Vector2 end	= m_rect.GetEnd() - Vector2(m_props.outlineThickness, m_props.outlineThickness);
			const Vector2 sz	= end - start;

			m_lvg->DrawRect(start.AsLVG(), Vector2(start.x + sz.x * perc, end.y).AsLVG(), fill, 0.0f, m_drawOrder + 1);
		}

		if (m_isEditing)
		{
			// Caret
			LinaVG::StyleOptions caret;
			m_props.colorCaret.w		   = m_caretAlpha;
			caret.color					   = m_props.colorCaret.AsLVG4();
			const Vector2 caretMiddle	   = GetPosFromCaretIndex(m_caretInsertPos);
			Vector2		  caretTopLeft	   = Vector2(caretMiddle.x - m_props.horizontalIndent * 0.1f, GetCaretStartY());
			Vector2		  caretBottomRight = Vector2(caretMiddle.x + m_props.horizontalIndent * 0.1f, GetCaretEndY());

			caretTopLeft.x	   = Math::Clamp(caretTopLeft.x, m_rect.pos.x, m_rect.GetEnd().x);
			caretBottomRight.x = Math::Clamp(caretBottomRight.x, m_rect.pos.x, m_rect.GetEnd().x);

			m_lvg->DrawRect(caretTopLeft.AsLVG(), caretBottomRight.AsLVG(), caret, 0, m_drawOrder);

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
				Vector2				 topLeft		   = Vector2(highlightStartMid.x, GetCaretStartY());
				Vector2				 bottomRight	   = Vector2(highlightEndMid.x, GetCaretEndY());
				LinaVG::StyleOptions highlight;
				highlight.color = m_props.colorHighlight.AsLVG4();

				topLeft.x	  = Math::Clamp(topLeft.x, m_rect.pos.x, m_rect.GetEnd().x);
				bottomRight.x = Math::Clamp(bottomRight.x, m_rect.pos.x, m_rect.GetEnd().x);
				m_lvg->DrawRect(topLeft.AsLVG(), bottomRight.AsLVG(), highlight, 0, m_drawOrder + 1);
			}
		}
	}

	void InputField::StartEditing()
	{
		m_manager->GrabControls(this);

		m_isEditing = true;
		PropagateCBOnEditStarted();
	}

	void InputField::UpdateTextFromValue()
	{
		m_text->GetProps().text = UtilStr::FloatToString(GetValue(), m_props.decimals);
		m_text->CalculateTextSize();
	}

	const String& InputField::GetValueStr() const
	{
		return m_text->GetProps().text;
	}

	void InputField::SelectAll()
	{
		m_caretInsertPos	= static_cast<uint32>(m_text->GetProps().text.size());
		m_highlightStartPos = 0;
	}

	void InputField::EndEditing(bool apply)
	{
		m_highlightStartPos = 0;
		m_caretInsertPos	= 0;
		m_isEditing			= false;
		m_textOffset		= 0.0f;

		if (!apply)
			return;

		if (m_props.valueStr && !m_props.isNumberField)
			*m_props.valueStr = m_text->GetProps().text;

		PropagateCBOnEditEnded();
	}

	Vector2 InputField::GetPosFromCaretIndex(uint32 index)
	{
		const float y = m_rect.pos.y + m_rect.size.y * 0.5f;

		if (index == 0)
			return Vector2(m_textStart.x, y);

		const String		substr = index == static_cast<uint32>(m_text->GetProps().text.size()) ? m_text->GetProps().text : m_text->GetProps().text.substr(0, index);
		LinaVG::TextOptions opts;
		opts.font		   = m_text->GetLVGFont();
		opts.textScale	   = m_text->GetProps().textScale;
		const Vector2 size = m_lvg->CalculateTextSize(substr.c_str(), opts);
		return Vector2(m_textStart.x + size.x, y);
	}

	uint32 InputField::GetCaretPosFromMouse()
	{
		const Vector2& mp  = m_manager->GetMousePosition();
		const float	   x   = mp.x - m_textStart.x;
		float		   off = Math::FloorToFloat(x / m_averageCharacterStep);
		if (off < 0.0f)
			off = 0.0f;

		const uint32 max		  = static_cast<uint32>(m_text->GetProps().text.size());
		const uint32 averageIndex = Math::Clamp((uint32)off, (uint32)0, max);

		const uint32 start = Math::Clamp(averageIndex - 5, (uint32)0, max);
		const uint32 end   = Math::Clamp(averageIndex + 5, (uint32)0, max);

		for (uint32 i = start; i < end; i++)
		{
			const String substr = m_text->GetProps().text.substr(0, i);

			LinaVG::TextOptions textOptions;
			textOptions.font	  = m_text->GetLVGFont();
			textOptions.textScale = m_text->GetProps().textScale;
			const Vector2 size	  = m_lvg->CalculateTextSize(substr.c_str(), textOptions);

			if (size.x > x - m_averageCharacterStep * 0.5f)
				return i;
		}

		return averageIndex;
	}

	bool InputField::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
	{
		if (!m_isEditing)
		{
			if (m_manager->IsControlsOwner(this) && keycode == LINAGX_KEY_RETURN && action != LinaGX::InputAction::Released)
			{
				StartEditing();
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
				EndEditing(true);
				return true;
			}

			if (keycode == LINAGX_KEY_ESCAPE)
			{
				EndEditing(false);
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
					m_caretInsertPos += static_cast<uint32>(str.size());
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
			{
				m_caretInsertPos--;
				m_highlightStartPos = m_caretInsertPos;
			}
			if (keycode == LINAGX_KEY_RIGHT)
			{
				m_caretInsertPos++;
				m_highlightStartPos = m_caretInsertPos;
			}
			ClampCaretInsert();
			if (!highlight)
				m_highlightStartPos = m_caretInsertPos;

			return true;
		}

		return false;
	}

	bool InputField::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (m_isEditing && !GetIsHovered())
		{
			EndEditing(true);
			return false;
		}

		// Catch middle press
		if (m_props.isNumberField && button == LINAGX_MOUSE_MIDDLE && action == LinaGX::InputAction::Pressed && m_isHovered)
		{
			if (m_props.valuePtr)
				m_valueOnMiddlePress = GetValue();

			PropagateCBOnEditStarted();

			m_middlePressed = true;
			return true;
		}

		// Catch middle release
		if (m_props.isNumberField && button == LINAGX_MOUSE_MIDDLE && action == LinaGX::InputAction::Released && m_middlePressed)
		{
			if (m_props.valuePtr && !Math::Equals(m_valueOnMiddlePress, GetValue(), 0.001f))
				PropagateCBOnEditEnded();

			m_middlePressed = false;
			return true;
		}

		if (m_isHovered && m_props.onRightClick && button == LINAGX_MOUSE_1 && action == LinaGX::InputAction::Pressed)
		{
			m_props.onRightClick();
			return true;
		}

		if (button != LINAGX_MOUSE_0)
			return false;

		// Double clicks -> select all if editing.
		// Start editing if number slider.
		if (m_isHovered && action == LinaGX::InputAction::Repeated)
		{
			SelectAll();

			if (m_isPressed)
				m_isPressed = false;

			return true;
		}

		if (action == LinaGX::InputAction::Pressed && m_isHovered)
		{
			m_caretInsertPos	= GetCaretPosFromMouse();
			m_highlightStartPos = m_caretInsertPos;
			m_isPressed			= true;
			StartEditing();
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

		if (m_props.isNumberField && m_props.valuePtr)
		{
			const String& str		   = m_text->GetProps().text;
			uint32		  outPrecision = 0;

			SetValue(UtilStr::StringToFloat(str, outPrecision));
			if (m_props.clampNumber)
			{
				SetValue(Math::Clamp(GetValue(), m_props.valueMin, m_props.valueMax));
				m_lastStoredValue = INPF_VALUE_MIN;
			}

			PropagateCBOnEdited();
		}

		Widget::PropagateCBOnEdited();
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

		if (m_text->GetProps().text.empty())
			m_textOffset = 0.0f;

		if (m_props.isNumberField && m_props.valuePtr)
		{
			const String& str		   = m_text->GetProps().text;
			uint32		  outPrecision = 0;

			const float value = UtilStr::StringToFloat(str, outPrecision);
			SetValue(value);
			if (m_props.clampNumber)
			{
				SetValue(Math::Clamp(GetValue(), m_props.valueMin, m_props.valueMax));
				m_lastStoredValue = INPF_VALUE_MIN;
			}
		}

		Widget::PropagateCBOnEdited();
	}

	void InputField::ClampCaretInsert()
	{
		m_caretInsertPos	= Math::Clamp(m_caretInsertPos, (uint32)0, static_cast<uint32>(m_text->GetProps().text.size()));
		m_highlightStartPos = Math::Clamp(m_highlightStartPos, (uint32)0, static_cast<uint32>(m_text->GetProps().text.size()));
	}
} // namespace Lina
