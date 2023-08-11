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

#ifndef WidgetProgressBar_HPP
#define WidgetProgressBar_HPP

#include "Widget.hpp"
#include "Math/Color.hpp"

namespace Lina
{
	class Vector2;
}

namespace Lina::Editor
{
	class WidgetProgressBar : public Widget
	{
	public:
		WidgetProgressBar(LinaGX::Window* window) : Widget(window){};
		virtual ~WidgetProgressBar() = default;

		void Draw(int threadID, const Vector2& start, const Vector2& size, const Vector2& textPos, float progress, const char* text);

		Color BackgroundColor	   = Color::White;
		Color ForegroundStartColor = Color::DarkBlue;
		Color ForegroundEndColor   = Color::DarkBlue;
		float Progress			   = 0.0f;
	};
} // namespace Lina::Editor

#endif
