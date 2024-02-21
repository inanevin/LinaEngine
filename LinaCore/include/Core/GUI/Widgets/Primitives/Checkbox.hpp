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
#include "Core/GUI/Theme.hpp"
#include "Common/Data/Functional.hpp"

namespace Lina
{
	class Font;
	class Icon;

	class Checkbox : public Widget
	{
	public:
		struct Properties
		{
			Delegate<void()> onClicked;
			TBLR			 margins			 = {};
			StringID		 font				 = Theme::GetDef().defaultFont;
			String			 checkIcon			 = "";
			float			 iconScale			 = 1.0f;
			Vector2			 iconOffsetPerc		 = Vector2();
			Color			 colorBackground	 = Theme::GetDef().background0;
			float			 rounding			 = Theme::GetDef().baseRounding;
			float			 outlineThickness	 = Theme::GetDef().baseOutlineThickness;
			Color			 colorOutline		 = Theme::GetDef().silent;
			Color			 colorOutlineChecked = Theme::GetDef().accentPrimary;
			bool			 isChecked			 = false;
		};

		Checkbox() : Widget(1){};
		virtual ~Checkbox() = default;

		virtual void Construct() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;

		void SetProps(const Properties& props);

		inline const Properties& GetProps() const
		{
			return m_props;
		}

	private:
		Icon*	   m_icon			  = nullptr;
		Properties m_props			  = {};
		Color	   m_usedOutlineColor = Color();
	};
} // namespace Lina
