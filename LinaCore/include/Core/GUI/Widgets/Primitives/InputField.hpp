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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Common/Data/String.hpp"
#include "Common/Data/Bitmask.hpp"

namespace Lina
{
	class Text;
	class Icon;

#define INPF_VALUE_MIN -99999
#define INPF_VALUE_MAX 99999

	class InputField : public Widget
	{
	public:
		InputField() : Widget(WF_CONTROLLABLE)
		{
		}
		virtual ~InputField() = default;

		struct Properties
		{
			Delegate<void()> onRightClick;

			Color	  colorHighlight	  = Theme::GetDef().accentPrimary1;
			Color	  colorCaret		  = Theme::GetDef().foreground0;
			ColorGrad colorNumberFill	  = {Theme::GetDef().accentPrimary1, Theme::GetDef().accentPrimary0};
			Color	  colorPlaceHolder	  = Theme::GetDef().silent1;
			Color	  colorTextDefault	  = Theme::GetDef().foreground0;
			float	  outlineThickness	  = Theme::GetDef().baseOutlineThickness;
			float	  horizontalIndent	  = Theme::GetDef().baseIndentInner;
			String	  placeHolderText	  = "";
			String	  placeHolderIcon	  = "";
			bool	  usePlaceHolder	  = false;
			bool	  isNumberField		  = false;
			bool	  disableNumberSlider = false;
			bool	  clampNumber		  = false;
			float	  valueMin			  = 0.0f;
			float	  valueMax			  = 10.0f;
			float	  valueStep			  = 0.1f;
			uint32	  decimals			  = 3;
			uint8*	  valuePtr			  = nullptr;
			uint8	  valueBits			  = 32;
			bool	  valueUnsigned		  = false;
			String*	  valueStr			  = nullptr;

			bool centerText = false;

			void SaveToStream(OStream& stream) const
			{
				stream << colorHighlight << colorCaret << colorNumberFill << colorPlaceHolder << colorTextDefault;
				stream << placeHolderText;
				stream << outlineThickness << horizontalIndent << usePlaceHolder << isNumberField << disableNumberSlider << clampNumber;
				stream << valueMin << valueMax << valueStep << centerText << decimals;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> colorHighlight >> colorCaret >> colorNumberFill >> colorPlaceHolder >> colorTextDefault;
				stream >> placeHolderText;
				stream >> outlineThickness >> horizontalIndent >> usePlaceHolder >> isNumberField >> disableNumberSlider >> clampNumber;
				stream >> valueMin >> valueMax >> valueStep >> centerText >> decimals;
			}
		};

		virtual void  Construct() override;
		virtual void  Initialize() override;
		virtual void  PreTick() override;
		virtual void  Tick(float delta) override;
		virtual void  Draw() override;
		virtual bool  OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action) override;
		virtual bool  OnMouse(uint32 button, LinaGX::InputAction action) override;
		void		  SelectAll();
		void		  StartEditing();
		void		  UpdateTextFromValue();
		const String& GetValueStr() const;

		virtual void OnGrabbedControls(bool fw, Widget* w) override
		{
			StartEditing();
			SelectAll();
		}

		virtual void OnLostControls(Widget* newOwner) override
		{
			if (m_isEditing)
				EndEditing(true);
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline Text* GetText()
		{
			return m_text;
		}

		inline virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			m_props.SaveToStream(stream);
		}

		inline virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			m_props.LoadFromStream(stream);
		}

		void SetValue(float val)
		{
			if (m_props.valueUnsigned)
			{
				if (m_props.valueBits == 32)
					*reinterpret_cast<uint32*>(m_props.valuePtr) = static_cast<uint32>(val);
				else if (m_props.valueBits == 16)
					*reinterpret_cast<uint16*>(m_props.valuePtr) = static_cast<uint16>(val);
				else if (m_props.valueBits == 8)
					*reinterpret_cast<uint8*>(m_props.valuePtr) = static_cast<uint8>(val);
			}
			else
			{
				if (m_props.valueBits == 32)
					*reinterpret_cast<float*>(m_props.valuePtr) = static_cast<float>(val);
				else if (m_props.valueBits == 16)
					*reinterpret_cast<int16*>(m_props.valuePtr) = static_cast<int16>(val);
				else if (m_props.valueBits == 8)
					*reinterpret_cast<int8*>(m_props.valuePtr) = static_cast<int8>(val);
			}
		}

		float GetValue()
		{
			if (m_props.valueUnsigned)
			{
				if (m_props.valueBits == 32)
					return static_cast<float>(*reinterpret_cast<uint32*>(m_props.valuePtr));
				else if (m_props.valueBits == 16)
					return static_cast<float>(*reinterpret_cast<uint16*>(m_props.valuePtr));
				else if (m_props.valueBits == 8)
					return static_cast<float>(*reinterpret_cast<uint8*>(m_props.valuePtr));
			}
			else
			{
				if (m_props.valueBits == 32)
					return static_cast<float>(*reinterpret_cast<float*>(m_props.valuePtr));
				else if (m_props.valueBits == 16)
					return static_cast<float>(*reinterpret_cast<int16*>(m_props.valuePtr));
				else if (m_props.valueBits == 8)
					return static_cast<float>(*reinterpret_cast<int8*>(m_props.valuePtr));
			}
			return 0.0f;
		}

	private:
		void	EndEditing(bool apply);
		uint32	GetCaretPosFromMouse();
		Vector2 GetPosFromCaretIndex(uint32 index);
		float	GetCaretStartY();
		float	GetCaretEndY();
		void	Insert(uint32 pos, const String& str);
		void	RemoveCurrent();
		void	ClampCaretInsert();

	private:
		LINA_REFLECTION_ACCESS(InputField);

		Properties m_props				  = {};
		Text*	   m_text				  = nullptr;
		Text*	   m_placeholderText	  = nullptr;
		Icon*	   m_placeholderIcon	  = nullptr;
		Vector2	   m_textStart			  = Vector2::Zero;
		Vector2	   m_textEnd			  = Vector2::Zero;
		uint32	   m_caretInsertPos		  = 0;
		uint32	   m_highlightStartPos	  = 0;
		float	   m_caretAlpha			  = 1.0f;
		float	   m_caretCtr			  = 0.0f;
		float	   m_averageCharacterStep = 0.0f;
		float	   m_textOffset			  = 0.0f;
		bool	   m_middlePressed		  = false;
		bool	   m_isEditing			  = false;
		float	   m_lastStoredValue	  = INPF_VALUE_MIN;
		float	   m_valueOnMiddlePress	  = 0.0f;
	};

	LINA_WIDGET_BEGIN(InputField, Primitive)
	LINA_FIELD(InputField, m_props, "", FieldType::UserClass, GetTypeID<InputField::Properties>());
	LINA_CLASS_END(InputField)

	LINA_CLASS_BEGIN(InputFieldProperties)
	LINA_FIELD(InputField::Properties, usePlaceHolder, "Use Placeholder", FieldType::Boolean, 0)
	LINA_FIELD(InputField::Properties, placeHolderText, "Placeholder", FieldType::String, 0)
	LINA_FIELD(InputField::Properties, decimals, "Decimals", FieldType::UInt32, 0)
	LINA_FIELD(InputField::Properties, valueBits, "Bits", FieldType::UInt8, 0)
	LINA_FIELD(InputField::Properties, valueUnsigned, "Unsigned", FieldType::Boolean, 0)
	LINA_FIELD(InputField::Properties, isNumberField, "Number Field", FieldType::Boolean, 0)
	LINA_FIELD(InputField::Properties, disableNumberSlider, "Disable Slider", FieldType::Boolean, 0)
	LINA_FIELD(InputField::Properties, clampNumber, "Clamp", FieldType::Boolean, 0)
	LINA_FIELD(InputField::Properties, valueMin, "Min", FieldType::Float, 0)
	LINA_FIELD(InputField::Properties, valueMax, "Max", FieldType::Float, 0)
	LINA_FIELD(InputField::Properties, valueStep, "Step", FieldType::Float, 0)
	LINA_FIELD(InputField::Properties, colorHighlight, "Highlight Color", FieldType::Color, 0)
	LINA_FIELD(InputField::Properties, colorCaret, "Caret Color", FieldType::Color, 0)
	LINA_FIELD(InputField::Properties, colorNumberFill, "Fill Color", FieldType::ColorGrad, 0)
	LINA_FIELD(InputField::Properties, colorPlaceHolder, "PlaceHolder Color", FieldType::Color, 0)
	LINA_FIELD(InputField::Properties, outlineThickness, "Outline Thickness", FieldType::Float, 0)
	LINA_FIELD(InputField::Properties, horizontalIndent, "Horizontal Indent", FieldType::Float, 0)

	LINA_FIELD_DEPENDENCY(InputField::Properties, placeHolderText, "usePlaceHolder", "1")
	LINA_FIELD_DEPENDENCY(InputField::Properties, disableNumberSlider, "isNumberField", "1")
	LINA_FIELD_DEPENDENCY(InputField::Properties, clampNumber, "isNumberField", "1")
	LINA_FIELD_DEPENDENCY(InputField::Properties, valueMin, "isNumberField", "1")
	LINA_FIELD_DEPENDENCY(InputField::Properties, valueMax, "isNumberField", "1")
	LINA_FIELD_DEPENDENCY(InputField::Properties, valueStep, "isNumberField", "1")
	LINA_CLASS_END(InputFieldProperties)

} // namespace Lina
