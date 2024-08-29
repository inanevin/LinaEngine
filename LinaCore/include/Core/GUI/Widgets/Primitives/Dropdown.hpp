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

	class Dropdown : public Widget
	{
	public:
		Dropdown() : Widget(WF_CONTROLLABLE)
		{
		}
		virtual ~Dropdown() = default;

		struct Properties
		{
			Delegate<bool(int32)>											 onSelected;
			Delegate<void(Vector<String>& outItems, Vector<int32>& outSelectedItem)> onAddItems;

			float horizontalIndent = Theme::GetDef().baseIndentInner;
            bool closeOnSelect = true;
            bool switchTextOnSelect = true;
            
			void SaveToStream(OStream& stream) const
			{
				stream << horizontalIndent;
                stream << closeOnSelect;
                stream << switchTextOnSelect;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> horizontalIndent;
                stream >> closeOnSelect;
                stream >> switchTextOnSelect;
			}
		};

		virtual void Construct() override;
		virtual void Tick(float delta) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action) override;

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
		Properties m_props		 = {};
		Icon*	   m_icon		 = nullptr;
		Text*	   m_text		 = nullptr;
		Vector2	   m_iconBgStart = Vector2::Zero;
	};

	LINA_REFLECTWIDGET_BEGIN(Dropdown, Primitive)
	LINA_REFLECTWIDGET_END(Dropdown)

} // namespace Lina
