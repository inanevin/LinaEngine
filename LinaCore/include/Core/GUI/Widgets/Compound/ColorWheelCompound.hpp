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

namespace Lina
{

	class ColorSlider;
	class DirectionalLayout;
	class Stack;
	class ColorWheel;
	class InputField;
	class Text;

	class ColorWheelCompound : public Widget
	{
	public:
		ColorWheelCompound()		  = default;
		virtual ~ColorWheelCompound() = default;

		static constexpr float TOP_RECT_RATIO = 0.6f;
		static constexpr float WHEEL_SIZE	  = 350.0f;

		struct Properties
		{
		};

		virtual void Construct() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;

	private:
		void SetSaturationValueLayout(DirectionalLayout*& layout, Text*& text, InputField*& field, ColorSlider*& slider, const String& label, float* val);

	private:
		Properties	 m_props			= {};
		Rect		 m_topRect			= Rect();
		Rect		 m_botRect			= Rect();
		Rect		 m_colorWheelRect	= Rect();
		Rect		 m_svRect			= Rect();
		ColorSlider* m_sliderSaturation = nullptr;
		ColorSlider* m_sliderValue		= nullptr;

		float m_saturation = 0.0f;
		float m_value	   = 0.0f;

		DirectionalLayout* m_topHorizontalLayout = nullptr;
		Stack*			   m_wheelStack			 = nullptr;

		ColorWheel*		   m_wheel			  = nullptr;
		DirectionalLayout* m_layoutSaturation = nullptr;
		DirectionalLayout* m_layoutValue	  = nullptr;
		InputField*		   m_fieldSaturation  = nullptr;
		InputField*		   m_fieldValue		  = nullptr;
		Text*			   m_labelSaturation  = nullptr;
		Text*			   m_labelValue		  = nullptr;
		DirectionalLayout* m_topRow			  = nullptr;
		DirectionalLayout* m_topSlidersRow	  = nullptr;
	};
} // namespace Lina
