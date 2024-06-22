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
#include "Common/Common.hpp"

namespace Lina
{
	class ColorSlider : public Widget
	{
	public:
		ColorSlider() : Widget(WF_CONTROLLABLE)
		{
		}
		virtual ~ColorSlider() = default;

		struct Properties
		{
			Delegate<void(float)> onValueChanged;
			DirectionOrientation  direction				  = DirectionOrientation::Horizontal;
			Color				  colorBegin			  = Color::White;
			Color				  colorEnd				  = Color::White;
			Color				  colorOutline			  = Theme::GetDef().outlineColorBase;
			Color				  colorOutlineControls	  = Theme::GetDef().outlineColorControls;
			Color				  colorLine				  = Theme::GetDef().foreground0;
			Color				  colorLineOutline		  = Theme::GetDef().background0;
			bool				  isHueShift			  = false;
			bool				  drawCheckeredBackground = false;
			float*				  value					  = nullptr;
			float				  rounding				  = Theme::GetDef().baseRounding;
			float				  outlineThickness		  = Theme::GetDef().baseOutlineThickness;
			float				  minValue				  = 0.0f;
			float				  maxValue				  = 0.0f;
			float				  step					  = 0.0f;

			void SaveToStream(OStream& stream) const
			{
				colorBegin.SaveToStream(stream);
				colorEnd.SaveToStream(stream);
				colorOutline.SaveToStream(stream);
				colorOutlineControls.SaveToStream(stream);
				colorLine.SaveToStream(stream);
				stream << isHueShift << drawCheckeredBackground << rounding << outlineThickness << minValue << maxValue << step;
				stream << static_cast<uint8>(direction);
			}

			void LoadFromStream(IStream& stream)
			{
				colorBegin.LoadFromStream(stream);
				colorEnd.LoadFromStream(stream);
				colorOutline.LoadFromStream(stream);
				colorOutlineControls.LoadFromStream(stream);
				colorLine.LoadFromStream(stream);
				stream >> isHueShift >> drawCheckeredBackground >> rounding >> outlineThickness >> minValue >> maxValue >> step;
				uint8 dir = 0;
				stream >> dir;
				direction = static_cast<DirectionOrientation>(dir);
			}
		};

		virtual void PreTick() override;
		virtual void Draw() override;
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

	private:
		Properties m_props = {};
	};

	LINA_REFLECTWIDGET_BEGIN(ColorSlider)
	LINA_REFLECTWIDGET_END(ColorSlider)

} // namespace Lina
