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

	class ColorField : public Widget, public LinaGX::WindowListener
	{
	public:
		ColorField() : Widget(0)
		{
		}
		virtual ~ColorField() = default;

		struct Properties
		{
			Delegate<void()> onClicked;
			Color*			 colorValue			  = nullptr;
			Color			 colorOutline		  = Color::White;
			Color			 colorOutlineControls = Color::White;
			float			 hoverHighlightPerc	  = 0.0f;
			float			 rounding			  = 0.0f;
			float			 outlineThickness	  = 0.0f;
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

	private:
		Properties m_props = {};
	};

} // namespace Lina
