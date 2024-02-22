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
	class Icon;
	class Text;
	class Popup;

	class Dropdown : public Widget, public LinaGX::WindowListener
	{
	public:
		Dropdown() : Widget(2)
		{
		}
		virtual ~Dropdown() = default;

		struct Properties
		{
			Delegate<void(Popup* popup)> onPopupCreated;
			Color						 colorBackground			= Color::White;
			Color						 colorHovered				= Color::White;
			Color						 colorOutline				= Color::White;
			Color						 colorOutlineControls		= Color::White;
			Color						 colorIconBackgroundStart	= Color::White;
			Color						 colorIconBackgroundEnd		= Color::White;
			Color						 colorIconBackgroundHovered = Color::White;
			float						 rounding					= 0.0f;
			float						 outlineThickness			= 0.0f;
			float						 horizontalIndent			= 0.0f;
			float						 verticalIndent				= 0.0f;
		};

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void Tick(float delta) override;
		virtual void Draw(int32 threadIndex) override;
		virtual void OnWindowMouse(uint32 button, LinaGX::InputAction action) override;

		inline Properties& GetProps()
		{
			return m_props;
		}

		inline Icon* GetIcon()
		{
			return m_icon;
		}

		inline Text* GetText()
		{
			return m_text;
		}

	private:
		Properties m_props		 = {};
		Icon*	   m_icon		 = nullptr;
		Text*	   m_text		 = nullptr;
		Vector2	   m_iconBgStart = Vector2::Zero;
		Popup*	   m_popup		 = nullptr;
	};

} // namespace Lina
