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
	class Popup : public Widget
	{
	public:
		Popup()			 = default;
		virtual ~Popup() = default;

		struct Properties
		{
			Delegate<void()> onDestructed;
			Color			 colorBackgroundStart = Theme::GetDef().background0;
			Color			 colorBackgroundEnd	  = Theme::GetDef().background2;
			Color			 colorOutline		  = Theme::GetDef().outlineColorBase;
			float			 outlineThickness	  = Theme::GetDef().baseOutlineThickness;
			float			 rounding			  = Theme::GetDef().baseRounding;
			float			 animTime			  = Theme::GetDef().popupAnimTime;
		};

		virtual void Destruct() override;
		virtual void Tick(float delta) override;
		virtual void Draw() override;

		inline Properties& GetProps()
		{
			return m_props;
		}

	private:
		Properties m_props		  = {};
		float	   m_animationCtr = 0.0f;
	};

	LINA_REFLECTWIDGET_BEGIN(Popup)
	LINA_REFLECTWIDGET_END(Popup)

} // namespace Lina
