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
#include "Common/Data/Functional.hpp"

namespace Lina
{
	class Icon;

	class Checkbox : public Widget
	{
	public:
		struct Properties
		{
			bool* value = nullptr;

			void SaveToStream(OStream& stream) const
			{
			}

			void LoadFromStream(IStream& stream)
			{
			}
		};

		Checkbox() : Widget(WF_CONTROLLABLE){};
		virtual ~Checkbox() = default;

		virtual void Construct() override;
		virtual void Tick(float delta) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act) override;

		virtual void SaveToStream(OStream& stream) const override
		{
			Widget::SaveToStream(stream);
			stream << m_props;
		}

		virtual void LoadFromStream(IStream& stream) override
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

	private:
		LINA_REFLECTION_ACCESS(Checkbox);
		Icon*	   m_icon  = nullptr;
		Properties m_props = {};
	};

	LINA_WIDGET_BEGIN(Checkbox, Primitive)
	LINA_FIELD(Checkbox, m_props, "", FieldType::UserClass, GetTypeID<Checkbox::Properties>());
	LINA_CLASS_END(Checkbox)

	LINA_CLASS_BEGIN(CheckboxProperties)
	LINA_CLASS_END(CheckboxProperties)
} // namespace Lina
