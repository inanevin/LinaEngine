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
#include "Common/Data/Functional.hpp"

namespace Lina
{
	class Font;
	class Text;

	class Button : public Widget
	{
	public:
		struct Properties
		{
			Delegate<void()> onClicked;
			Color			 colorDefaultStart	  = Theme::GetDef().background2;
			Color			 colorDefaultEnd	  = Theme::GetDef().background3;
			Color			 colorHovered		  = Theme::GetDef().background4;
			Color			 colorPressed		  = Theme::GetDef().background0;
			float			 rounding			  = Theme::GetDef().baseRounding;
			float			 outlineThickness	  = Theme::GetDef().baseOutlineThickness;
			Color			 colorOutline		  = Theme::GetDef().outlineColorBase;
			Color			 colorOutlineControls = Theme::GetDef().outlineColorControls;
		};

		Button() : Widget(1, WF_SELECTABLE){};
		virtual ~Button() = default;

		virtual void Construct() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		virtual bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act) override;
		virtual bool OnMouse(uint32 button, LinaGX::InputAction act) override;

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline Text* GetText()
		{
			return m_text;
		}

	private:
		Text*	   m_text  = nullptr;
		Properties m_props = {};
	};
} // namespace Lina
