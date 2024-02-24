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

#include "Core/GUI/Widgets/Compound/ColorWheelCompound.hpp"
#include "Core/GUI/Widgets/Primitives/ColorSlider.hpp"
#include "Core/GUI/Widgets/Primitives/ColorWheel.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/Stack.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	void ColorWheelCompound::SetSaturationValueLayout(DirectionalLayout*& layout, Text*& text, InputField*& field, ColorSlider*& slider, const String& label, float* val)
	{

		// Label
		text					   = Allocate<Text>();
		text->GetProps().text	   = label;
		text->GetProps().isDynamic = true;
		text->CalculateTextSize();
		text->GetFlags().Set(WF_EXPAND);
		text->SetDebugName("Label");

		// Field
		field							= Allocate<InputField>();
		field->GetProps().isNumberField = true;
		field->GetProps().clampNumber	= true;
		field->GetProps().numberMin		= 0.0f;
		field->GetProps().numberMax		= 1.0f;
		field->GetProps().numberStep	= 0.01f;
		field->GetProps().numberValue	= val;
		field->SetDebugName("Field");

		// Color slider
		slider						 = Allocate<ColorSlider>();
		slider->GetProps().value	 = val;
		slider->GetProps().direction = WidgetDirection::Vertical;
		// slider->GetFlags().Set(WF_EXPAND);
		slider->SetDebugName("ColorSlider");

		// Layout
		layout						 = Allocate<DirectionalLayout>();
		layout->GetProps().direction = WidgetDirection::Vertical;
		layout->GetProps().padding	 = Theme::GetDef().baseIndent;
		layout->AddChild(slider, field, text);
		layout->SetDebugName("DirectionalLayout");
	}

	void ColorWheelCompound::Construct()
	{
		// Wheel stack.
		m_wheel = Allocate<ColorWheel>();
		m_wheel->SetDebugName("Wheel");
		m_wheelStack							  = Allocate<Stack>();
		m_wheelStack->GetProps().controlChildSize = true;
		m_wheelStack->GetProps().margins		  = TBLR::Eq(Theme::GetDef().baseIndent * 2.0f);
		m_wheelStack->SetDebugName("WheelStack");
		m_wheelStack->AddChild(m_wheel);

		// Sliders
		SetSaturationValueLayout(m_layoutSaturation, m_labelSaturation, m_fieldSaturation, m_sliderSaturation, "SAB", &m_saturation);
		SetSaturationValueLayout(m_layoutValue, m_labelValue, m_fieldValue, m_sliderValue, "V", &m_value);

		// Slider row
		m_topSlidersRow									 = Allocate<DirectionalLayout>();
		m_topSlidersRow->GetProps().direction			 = WidgetDirection::Horizontal;
		m_topSlidersRow->GetProps().margins				 = TBLR::Eq(Theme::GetDef().baseIndent * 2.0f);
		m_topSlidersRow->GetProps().controlCrossAxisSize = true;
		m_topSlidersRow->GetProps().padding				 = Theme::GetDef().baseIndent;
		m_topSlidersRow->GetFlags().Set(WF_EXPAND);
		m_topSlidersRow->AddChild(m_layoutSaturation, m_layoutValue);
		m_topSlidersRow->SetDebugName("TopSliderRow");

		// Top row
		m_topRow								  = Allocate<DirectionalLayout>();
		m_topRow->GetProps().direction			  = WidgetDirection::Horizontal;
		m_topRow->GetProps().controlCrossAxisSize = true;
		m_topRow->AddChild(m_wheelStack, m_topSlidersRow);
		m_topRow->SetDebugName("TopRow");

		AddChild(m_topRow);

		// m_topHorizontalLayout = Allocate<DirectionalLayout>();
		// AddChild(m_topHorizontalLayout);
		// m_saturationSlider						 = Allocate<ColorSlider>();
		// m_saturationSlider->GetProps().direction = WidgetDirection::Vertical;
		// m_saturationSlider->GetProps().value	 = &m_saturation;
		// m_valueSlider							 = Allocate<ColorSlider>();
		// m_valueSlider->GetProps().direction		 = WidgetDirection::Vertical;
		// m_valueSlider->GetProps().value			 = &m_value;
		// AddChild(m_saturationSlider);
		// AddChild(m_valueSlider);
	}

	void ColorWheelCompound::Tick(float delta)
	{
		Widget::SetIsHovered();
		const float baseItemHeight = Theme::GetBaseItemHeight(m_lgxWindow->GetDPIScale());
		// Expand.
		m_wheelStack->SetSizeX(WHEEL_SIZE);

		m_sliderSaturation->SetSize(Vector2(baseItemHeight, 88.0f));
		m_sliderValue->SetSize(Vector2(baseItemHeight, 88.0f));
		m_fieldSaturation->SetSize(Vector2(baseItemHeight * 2, baseItemHeight));
		m_fieldValue->SetSize(Vector2(baseItemHeight * 2, baseItemHeight));
		m_layoutSaturation->SetSizeX(baseItemHeight * 2);
		m_layoutValue->SetSizeX(baseItemHeight * 2);
		// m_labelSaturation->SetSizeY(0.0f);

		m_topRow->SetPos(m_rect.pos);
		m_topRow->SetSize(Vector2(m_rect.size.x, WHEEL_SIZE));
		m_topRow->Tick(delta);

		// m_wheelStack->SetPos(m_rect.pos);
		// m_wheelStack->SetSize(Vector2(300, 300));
		// m_wheelStack->Tick(delta);

		return;

		// Calc rects.
		const float colorWheelRectIndent = Theme::GetDef().baseIndent * 2;

		m_colorWheelRect.pos  = Vector2(m_rect.pos.x + colorWheelRectIndent, m_rect.pos.y + colorWheelRectIndent);
		m_colorWheelRect.size = Vector2(WHEEL_SIZE, WHEEL_SIZE);

		m_svRect.pos  = Vector2(m_rect.pos.x + colorWheelRectIndent * 2 + WHEEL_SIZE, m_rect.pos.y + colorWheelRectIndent);
		m_svRect.size = Vector2(m_rect.pos.x + m_rect.size.x, m_rect.pos.y + colorWheelRectIndent + WHEEL_SIZE) - m_svRect.pos;

		m_topRect.pos  = Vector2(m_rect.pos.x, m_rect.pos.y);
		m_topRect.size = Vector2(m_rect.size.x, m_rect.size.y * TOP_RECT_RATIO);
		m_botRect.pos  = Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y * TOP_RECT_RATIO);
		m_botRect.size = Vector2(m_rect.size.x, m_rect.size.y * (1.0f - TOP_RECT_RATIO));

		m_colorWheelRect.pos  = Vector2(m_rect.pos.x, m_rect.pos.y);
		m_colorWheelRect.size = Vector2(m_rect.size.y * TOP_RECT_RATIO, m_rect.size.y * TOP_RECT_RATIO);
		m_svRect.pos		  = Vector2(m_rect.pos.x + m_colorWheelRect.size.x, m_rect.pos.y);
		m_svRect.size		  = Vector2(m_rect.size.x - m_colorWheelRect.size.x, m_colorWheelRect.size.y);

		m_colorWheelRect.pos.x += colorWheelRectIndent;
		m_colorWheelRect.pos.y += colorWheelRectIndent;
		m_colorWheelRect.size.x -= colorWheelRectIndent * 2.0f;
		m_colorWheelRect.size.y -= colorWheelRectIndent * 2.0f;
		m_svRect.pos.y += colorWheelRectIndent;
		m_svRect.size.y -= colorWheelRectIndent * 2.0f;

		// Place saturation & value sliders
		// const float svSliderWidth = Theme::GetBaseItemHeight(m_lgxWindow->GetDPIScale());
		// const float svSliderHeight = m_svRect.size.y - Theme::GetDef().baseIndentInner * 2;
		//
		// const Vector2 saturationSliderBegin = Vector2(m_svRect.GetCenter().x - Theme::GetDef().baseIndentInner - svSliderWidth, m_svRect.GetCenter().y - svSliderHeight * 0.5f);
		// const Vector2 saturationSliderEnd = Vector2(m_svRect.GetCenter().x - Theme::GetDef().baseIndentInner, m_svRect.GetCenter().y + svSliderHeight * 0.5f);
		// m_saturationSlider->SetPos(saturationSliderBegin);
		// m_saturationSlider->SetSize(saturationSliderEnd - saturationSliderBegin);
		// m_saturationSlider->Tick(delta);
		//
		// const Vector2 valueSliderBegin = Vector2(m_svRect.GetCenter().x + Theme::GetDef().baseIndentInner, m_svRect.GetCenter().y - svSliderHeight * 0.5f);
		// const Vector2 valueSliderEnd = Vector2(m_svRect.GetCenter().x + Theme::GetDef().baseIndentInner + svSliderWidth, m_svRect.GetCenter().y + svSliderHeight * 0.5f);
		// m_valueSlider->SetPos(valueSliderBegin);
		// m_valueSlider->SetSize(valueSliderEnd - valueSliderBegin);
		// m_valueSlider->Tick(delta);
	}

	void ColorWheelCompound::Draw(int32 threadIndex)
	{
		m_topRow->Draw(threadIndex);

		// m_wheelStack->Draw(threadIndex);

		// // Color wheel.
		// LinaVG::StyleOptions wheelStyle;
		// //wheelStyle.textureHandle = GUI_TEXTURE_COLORWHEEL;
		// LinaVG::DrawRect(threadIndex, m_colorWheelRect.pos.AsLVG(), (m_colorWheelRect.pos + m_colorWheelRect.size).AsLVG(), wheelStyle, 0.0f, m_drawOrder);
		// //wheelStyle.color = Color::Red.AsLVG4();
		// LinaVG::DrawRect(threadIndex, m_svRect.pos.AsLVG(), (m_svRect.pos + m_svRect.size).AsLVG(), wheelStyle, 0.0f, m_drawOrder);

		// Saturation & value sliders.
		// m_saturationSlider->Draw(threadIndex);
	}
} // namespace Lina
