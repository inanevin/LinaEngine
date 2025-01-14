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
#include "Editor/Graphics/EditorGfxHelpers.hpp"

namespace Lina
{
	class Icon;
};

namespace Lina::Editor
{
	class ColorWheel : public Widget
	{
	public:
		ColorWheel()		  = default;
		virtual ~ColorWheel() = default;

		struct Properties
		{
			float*						 hue		   = nullptr;
			float*						 saturation	   = nullptr;
			float						 darknessAlpha = 1.0f;

			void SaveToStream(OStream& stream) const
			{
			}

			void LoadFromStream(IStream& stream)
			{
			}
		};

		virtual void Construct() override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;

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

	private:
		LINA_REFLECTION_ACCESS(ColorWheel);

		Properties	m_props		 = {};
		Icon*		m_icon		 = nullptr;
		Vector2		m_pointerPos = Vector2::Zero;
		GUIUserData m_guiUserData;
		GUIUserData m_guiUserDataIcon;
	};

	LINA_WIDGET_BEGIN(ColorWheel, Primitive)
	LINA_FIELD(ColorWheel, m_props, "", FieldType::UserClass, GetTypeID<ColorWheel::Properties>());
	LINA_CLASS_END(ColorWheel)

	LINA_CLASS_BEGIN(ColorWheelProperties)
	LINA_CLASS_END(ColorWheelProperties)
} // namespace Lina::Editor
