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
	class Dropdown;

	class ColorWheelCompound : public Widget
	{
	public:
		ColorWheelCompound()		  = default;
		virtual ~ColorWheelCompound() = default;

		static constexpr float WHEEL_STACK_PERC = 0.6f;

		struct Properties
		{
		};

		virtual void Construct() override;
		virtual void Initialize() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		void		 SetTargetColor(const Color& col);

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		struct ColorComponent
		{
			DirectionalLayout* row	  = nullptr;
			InputField*		   field  = nullptr;
			ColorSlider*	   slider = nullptr;
		};

		struct SaturationValueComponent
		{
			DirectionalLayout* layout = nullptr;
			ColorSlider*	   slider = nullptr;
			InputField*		   field  = nullptr;
			Text*			   text	  = nullptr;
		};

	private:
		SaturationValueComponent ConstructHSVComponent(const String& label, bool isHue);
		ColorComponent			 ConstructColorComponent(const String& label);
		void					 Recalculate(bool sourceRGB);

	private:
		Properties				 m_props			   = {};
		DirectionalLayout*		 m_topRow			   = nullptr;
		Stack*					 m_wheelStack		   = nullptr;
		ColorWheel*				 m_wheel			   = nullptr;
		DirectionalLayout*		 m_topSlidersRow	   = nullptr;
		DirectionalLayout*		 m_bottomRow		   = nullptr;
		SaturationValueComponent m_hueComponent		   = {};
		SaturationValueComponent m_saturationComponent = {};
		SaturationValueComponent m_valueComponent	   = {};
		ColorComponent			 m_colorComp1		   = {};
		ColorComponent			 m_colorComp2		   = {};
		ColorComponent			 m_colorComp3		   = {};
		ColorComponent			 m_colorComp4		   = {};
		Dropdown*				 m_displayDropdown	   = nullptr;

		ColorDisplay m_selectedDisplay = ColorDisplay::RGB;
		Color		 m_editedColor	   = Color::White;
		Color		 m_hsv			   = Color(0, 0, 0, 0);
	};
} // namespace Lina
