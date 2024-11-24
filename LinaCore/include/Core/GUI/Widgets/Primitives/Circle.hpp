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

	class Circle : public Widget
	{
	public:
		struct Properties
		{
			float	  startAngle	  = 0.0f;
			float	  endAngle		  = 360.0f;
			ColorGrad colorBackground = Theme::GetDef().background1;
			ColorGrad colorForeground = Theme::GetDef().foreground0;
			float	  thickness		  = Theme::GetDef().baseOutlineThickness;
			bool	  useAA			  = false;
			bool	  isFilled		  = false;
			float	  foregroundFill  = 0.0f;
			bool	  useXForRadius	  = false;

			void SaveToStream(OStream& stream) const
			{
				stream << startAngle << endAngle << colorBackground << colorForeground << thickness << useAA << isFilled << useXForRadius;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> startAngle >> endAngle >> colorBackground >> colorForeground >> thickness >> useAA >> isFilled >> useXForRadius;
			}
		};

		Circle() : Widget(){};
		virtual ~Circle() = default;

		virtual void Draw() override;

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
		LINA_REFLECTION_ACCESS(Circle);
		Properties m_props = {};
	};

	LINA_WIDGET_BEGIN(Circle, Primitive)
	LINA_FIELD(Circle, m_props, "", FieldType::UserClass, GetTypeID<Circle::Properties>());
	LINA_CLASS_END(Circle)

	LINA_CLASS_BEGIN(CircleProperties)
	LINA_CLASS_END(CircleProperties)
} // namespace Lina
