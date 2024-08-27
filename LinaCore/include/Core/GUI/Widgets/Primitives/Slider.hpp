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

namespace LinaVG
{
	struct StyleOptions;
}

namespace Lina
{
	class Icon;

	class Slider : public Widget
	{
	public:
		Slider() : Widget(WF_CONTROLLABLE){};
		virtual ~Slider() = default;

		struct Properties
		{
			DirectionOrientation direction			 = DirectionOrientation::Horizontal;
			Color				 colorFillMin		 = Theme::GetDef().accentPrimary1;
			Color				 colorFillMax		 = Theme::GetDef().accentPrimary0;
			float				 crossAxisPercentage = Theme::GetDef().baseSliderPerc;
			float				 minValue			 = 0.0f;
			float				 maxValue			 = 0.0f;
			float*				 valuePtr			 = nullptr;
			float				 step				 = 0.0f;
			float				 localValue			 = 0.0f;

			Delegate<void(float val)> onValueChanged;

			void SaveToStream(OStream& stream) const
			{
				colorFillMin.SaveToStream(stream);
				colorFillMax.SaveToStream(stream);
				stream << crossAxisPercentage << minValue << maxValue << step;
			}

			void LoadFromStream(IStream& stream)
			{
				colorFillMin.LoadFromStream(stream);
				colorFillMax.LoadFromStream(stream);
				stream >> crossAxisPercentage >> minValue >> maxValue >> step;
			}
		};

		virtual void Construct() override;
		virtual void PreTick() override;
		virtual void Tick(float delta) override;
		virtual void Draw() override;
		virtual bool OnMouse(uint32 mouse, LinaGX::InputAction action) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act) override;

		Properties& GetProps()
		{
			return m_props;
		}

		inline Icon* GetHandle()
		{
			return m_handle;
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
		void GetStartEnd(Vector2& outStart, Vector2& outEnd, float fillPercent);

	private:
		Properties m_props		= {};
		Icon*	   m_handle		= nullptr;
		Vector2	   m_start		= Vector2::Zero;
		Vector2	   m_end		= Vector2::Zero;
		Vector2	   m_pressStart = Vector2::Zero;

		Vector2 m_bgStart = Vector2::Zero;
		Vector2 m_bgEnd	  = Vector2::Zero;

		Vector2 m_fillStart = Vector2::Zero;
		Vector2 m_fillEnd	= Vector2::Zero;
	};

	LINA_REFLECTWIDGET_BEGIN(Slider, Primitive)
	LINA_REFLECTWIDGET_END(Slider)

} // namespace Lina
