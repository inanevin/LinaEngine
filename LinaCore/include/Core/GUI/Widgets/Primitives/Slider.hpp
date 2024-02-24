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
		Slider() : Widget(1)
		{
		}
		virtual ~Slider() = default;

		struct Properties
		{
			WidgetDirection direction			 = WidgetDirection::Horizontal;
			Color			colorBackground		 = Theme::GetDef().background0;
			Color			colorFillMin		 = Theme::GetDef().accentPrimary1;
			Color			colorFillMax		 = Theme::GetDef().accentPrimary0;
			Color			colorHandle			 = Theme::GetDef().accentPrimary0;
			Color			colorHandleHovered	 = Theme::GetDef().accentPrimary2;
			Color			colorOutline		 = Theme::GetDef().outlineColorBase;
			Color			colorOutlineControls = Theme::GetDef().outlineColorControls;
			float			rounding			 = Theme::GetDef().baseRounding * 2;
			float			crossAxisPercentage	 = Theme::GetDef().baseSliderPerc;
			float			outlineThickness	 = Theme::GetDef().baseOutlineThickness;
			float			minValue			 = 0.0f;
			float			maxValue			 = 0.0f;
			float*			currentValue		 = nullptr;
			float			step				 = 0.0f;
		};

		virtual void Construct() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		virtual bool OnMouse(uint32 mouse, LinaGX::InputAction action) override;

		Properties& GetProps()
		{
			return m_props;
		}

		inline Icon* GetHandle()
		{
			return m_handle;
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

} // namespace Lina
