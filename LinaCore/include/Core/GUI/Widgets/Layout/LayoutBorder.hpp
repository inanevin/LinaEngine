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
	class LayoutBorder : public Widget
	{
	public:
		LayoutBorder()			= default;
		virtual ~LayoutBorder() = default;

		struct Properties
		{
			DirectionOrientation orientation = DirectionOrientation::Horizontal;
			float				 minSize	 = 0.0f;
			float				 thickness	 = Theme::GetDef().baseBorderThickness;
		};

		virtual void			   Construct() override;
		virtual void			   Initialize() override;
		virtual void			   PreTick() override;
		virtual bool			   OnMouse(uint32 button, LinaGX::InputAction act) override;
		virtual LinaGX::CursorType GetCursorOverride() override;
		void					   AssignSides(Widget* negative, Widget* positive);
		virtual void			   SaveToStream(OStream& stream) const override;
		virtual void			   LoadFromStream(IStream& stream) override;

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline Widget* GetNegative()
		{
			return m_negative;
		}

		inline Widget* GetPositive()
		{
			return m_positive;
		}

	private:
		Properties m_props	   = {};
		Widget*	   m_negative  = nullptr;
		Widget*	   m_positive  = nullptr;
		float	   m_pressDiff = 0.0f;
	};

	LINA_WIDGET_BEGIN(LayoutBorder, Layout)
	LINA_CLASS_END(LayoutBorder)
} // namespace Lina
