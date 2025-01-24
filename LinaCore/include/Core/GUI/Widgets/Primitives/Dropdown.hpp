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

namespace Lina
{
	class Icon;
	class Text;
	class DirectionalLayout;
	class Popup;

	class Dropdown : public Widget
	{
	public:
		Dropdown() : Widget(WF_CONTROLLABLE)
		{
		}
		virtual ~Dropdown() = default;

		struct Properties
		{
			Delegate<bool(int32, String& outNewTitle)> onSelected;
			Delegate<void(Popup* popup)>			   onAddItems;

			float horizontalIndent = Theme::GetDef().baseIndentInner;
			bool  closeOnSelect	   = true;

			void SaveToStream(OStream& stream) const
			{
				stream << horizontalIndent;
				stream << closeOnSelect;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> horizontalIndent;
				stream >> closeOnSelect;
			}
		};

		virtual void Construct() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action) override;

		inline virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			stream << m_props;
		}

		inline virtual void LoadFromStream(IStream& stream) override
		{
			Widget::LoadFromStream(stream);
			stream >> m_props;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline Icon* GetIcon()
		{
			return m_icon;
		}

		inline Text* GetText()
		{
			return m_text;
		}

	private:
		void CreatePopup();

	private:
		LINA_REFLECTION_ACCESS(Dropdown);

		Properties m_props		   = {};
		Icon*	   m_icon		   = nullptr;
		Text*	   m_text		   = nullptr;
		Vector2	   m_iconBgStart   = Vector2::Zero;
		Widget*	   m_textContainer = nullptr;
	};

	LINA_WIDGET_BEGIN(Dropdown, Primitive)
	LINA_FIELD(Dropdown, m_props, "", FieldType::UserClass, GetTypeID<Dropdown::Properties>());
	LINA_CLASS_END(Dropdown)

	LINA_CLASS_BEGIN(DropdownProperties)
	LINA_FIELD(Dropdown::Properties, closeOnSelect, "Close On Select", FieldType::Boolean, 0)
	LINA_FIELD(Dropdown::Properties, horizontalIndent, "Horizontal Indent", FieldType::Float, 0)
	LINA_CLASS_END(DropdownProperties)

} // namespace Lina
