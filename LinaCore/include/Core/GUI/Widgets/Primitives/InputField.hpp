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
#include "Common/Serialization/StringSerialization.hpp"

namespace Lina
{
	class Text;

	class InputField : public Widget
	{
	public:
		InputField() : Widget(WF_CONTROLLABLE)
		{
		}
		virtual ~InputField() = default;

		struct Properties
		{
			Delegate<void(const String&)> onEditStarted;
			Delegate<void(const String&)> onEdited;
			Delegate<void(const String&)> onEditEnd;
			Delegate<void(float)>		  onValueChanged;

			Color  colorHighlight		= Theme::GetDef().accentPrimary1;
			Color  colorCaret			= Theme::GetDef().foreground0;
			Color  colorNumberFillStart = Theme::GetDef().accentPrimary1;
			Color  colorNumberFillEnd	= Theme::GetDef().accentPrimary0;
			Color  colorPlaceHolder		= Theme::GetDef().outlineColorBase;
			Color  colorTextDefault		= Theme::GetDef().foreground0;
			float  outlineThickness		= Theme::GetDef().baseOutlineThickness;
			float  horizontalIndent		= Theme::GetDef().baseIndentInner;
			String placeHolderText		= "";
			bool   usePlaceHolder		= false;
			bool   isNumberField		= false;
			bool   disableNumberSlider	= false;
			bool   clampNumber			= false;
			float* value				= nullptr;
			float  valueMin				= 0.0f;
			float  valueMax				= 10.0f;
			float  valueStep			= 0.0f;
			uint32 decimals				= 3;

			bool centerText = false;
			bool wrapText	= false;
			bool clipText	= true;

			void SaveToStream(OStream& stream) const
			{

				colorHighlight.SaveToStream(stream);
				colorCaret.SaveToStream(stream);
				colorNumberFillStart.SaveToStream(stream);
				colorNumberFillEnd.SaveToStream(stream);
				colorPlaceHolder.SaveToStream(stream);
				colorTextDefault.SaveToStream(stream);
				StringSerialization::SaveToStream(stream, placeHolderText);
				stream << outlineThickness << horizontalIndent << usePlaceHolder << isNumberField << disableNumberSlider << clampNumber;
				stream << valueMin << valueMax << valueStep << centerText << wrapText << clipText << decimals;
			}

			void LoadFromStream(IStream& stream)
			{
				colorHighlight.LoadFromStream(stream);
				colorCaret.LoadFromStream(stream);
				colorNumberFillStart.LoadFromStream(stream);
				colorNumberFillEnd.LoadFromStream(stream);
				colorPlaceHolder.LoadFromStream(stream);
				colorTextDefault.LoadFromStream(stream);
				stream >> outlineThickness >> horizontalIndent >> usePlaceHolder >> isNumberField >> disableNumberSlider >> clampNumber;
				stream >> valueMin >> valueMax >> valueStep >> centerText >> wrapText >> clipText >> decimals;
			}
		};

		virtual void			   Construct() override;
		virtual void			   CalculateSize(float delta) override;
		virtual void			   PreTick() override;
		virtual void			   Tick(float delta) override;
		virtual void			   Draw() override;
		virtual bool			   OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action) override;
		virtual bool			   OnMouse(uint32 button, LinaGX::InputAction action) override;
		virtual LinaGX::CursorType GetCursorOverride() override;
		void					   SelectAll();
		void					   StartEditing();

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

	private:
		void	EndEditing();
		uint32	GetCaretPosFromMouse();
		Vector2 GetPosFromCaretIndex(uint32 index);
		float	GetCaretStartY();
		float	GetCaretEndY();
		void	Insert(uint32 pos, const String& str);
		void	RemoveCurrent();
		void	ClampCaretInsert();

	private:
		Properties m_props				  = {};
		Text*	   m_text				  = nullptr;
		Text*	   m_placeholderText	  = nullptr;
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
		float	   m_lastStoredValue	  = -99999.0f;
	};

	LINA_REFLECTWIDGET_BEGIN(InputField, Primitive)
	LINA_REFLECTWIDGET_END(InputField)

} // namespace Lina
