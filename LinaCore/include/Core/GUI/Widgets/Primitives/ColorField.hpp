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

	class ColorField : public Widget
	{
	public:
		ColorField() : Widget(WF_CONTROLLABLE)
		{
		}
		virtual ~ColorField() = default;

		struct Properties
		{
			Delegate<void()> onClicked;
			Color*			 value					 = nullptr;
			bool			 drawCheckeredBackground = false;
			bool			 convertToLinear		 = false;
			Color			 colorBackground		 = Theme::GetDef().background0;
			Color			 colorOutline			 = Theme::GetDef().outlineColorBase;
			Color			 colorOutlineControls	 = Theme::GetDef().outlineColorControls;
			float			 hoverHighlightPerc		 = 0.1f;
			float			 rounding				 = Theme::GetDef().baseRounding;
			float			 outlineThickness		 = Theme::GetDef().baseOutlineThickness;

			void SaveToStream(OStream& stream) const
			{
				stream << drawCheckeredBackground << convertToLinear << hoverHighlightPerc << rounding << outlineThickness;
				colorBackground.SaveToStream(stream);
				colorOutline.SaveToStream(stream);
				colorOutlineControls.SaveToStream(stream);
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> drawCheckeredBackground >> convertToLinear >> hoverHighlightPerc >> rounding >> outlineThickness;
				colorBackground.LoadFromStream(stream);
				colorOutline.LoadFromStream(stream);
				colorOutlineControls.LoadFromStream(stream);
			}
		};

		virtual void Draw() override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction action) override;

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

	LINA_REFLECTWIDGET_BEGIN(ColorField)
	LINA_REFLECTWIDGET_END(ColorField)

} // namespace Lina
