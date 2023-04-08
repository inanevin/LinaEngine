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

#include "GUI/Widgets/WidgetProgressBar.hpp"
#include "Math/Vector.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/Theme.hpp"

using namespace Lina;

namespace Lina::Editor
{
	void WidgetProgressBar::Draw(int threadID, const Vector2& start, const Vector2& size, const Vector2& textPos, float progress, const char* text)
	{
		LinaVG::StyleOptions style;
		style.color = LV4(BackgroundColor);
		LinaVG::DrawRect(threadID, LV2(start), LV2((start + size)), style, 0.0f, 1);

		style.color.start			 = LV4(ForegroundStartColor);
		style.color.end				 = LV4(ForegroundEndColor);
		const Vector2 foregroundSize = Vector2(size.x * Progress, size.y);
		LinaVG::DrawRect(threadID, LV2(start), LV2((start + foregroundSize)), style, 0.0f, 1);

		LinaVG::TextOptions textStyle;
		textStyle.font = Theme::GetFont(FontType::DefaultEditor, GetDPIScale());
		LinaVG::DrawTextNormal(threadID, text, LV2(textPos), textStyle, 0.0f, 2);
	}
} // namespace Lina::Editor
