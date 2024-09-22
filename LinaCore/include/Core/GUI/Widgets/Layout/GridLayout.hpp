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
	class GridLayout : public Widget
	{
	public:
		GridLayout()		  = default;
		virtual ~GridLayout() = default;

		enum class BackgroundStyle
		{
			None,
			Default,
		};

		struct Properties
		{
			float verticalPadding = Theme::GetDef().baseIndent;

			void SaveToStream(OStream& stream) const
			{
				stream << verticalPadding;
			}

			void LoadFromStream(IStream& stream)
			{
				stream >> verticalPadding;
			}
		};

		virtual void Tick(float delta) override;

		virtual float CalculateChildrenSize() override
		{
			return m_totalChildHeight;
		}

		inline Properties& GetProps()
		{
			return m_props;
		}

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

	private:
		LINA_REFLECTION_ACCESS(GridLayout);

		Properties m_props			  = {};
		float	   m_totalChildHeight = 0.0f;
	};

	LINA_WIDGET_BEGIN(GridLayout, Layout)
	LINA_FIELD(GridLayout, m_props, "Properties", FieldType::UserClass, GetTypeID<GridLayout::Properties>())
	LINA_CLASS_END(GridLayout)

	LINA_CLASS_BEGIN(GridLayoutProperties)
	LINA_FIELD(GridLayout::Properties, verticalPadding, "Vertical Padding", FieldType::Float, 0)
	LINA_CLASS_END(GridLayoutProperties)
} // namespace Lina
