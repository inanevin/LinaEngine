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

#include "Core/GUI/CommonGUI.hpp"

namespace LinaVG
{
	class Drawer;
}

namespace Lina
{
	class Rect;
	class Widget;
	class Text;
	class DirectionalLayout;
	class Button;

	class WidgetUtility
	{
	public:
		static void DrawRectBackground(LinaVG::Drawer* lvg, const RectBackground& options, const Rect& rect, int32 drawOrder, bool positionIsCenter = false);
		static void DrawAlphaLine(LinaVG::Drawer* lvg, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& baseColor);
		static void DrawAlphaLineCentral(LinaVG::Drawer* lvg, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& baseColor);
		static void DrawGradLineCentral(LinaVG::Drawer* lvg, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& centerColor, const Color& edgeColor);
		static void DrawDropShadow(LinaVG::Drawer* lvg, const Vector2& p1, const Vector2& p2, int32 drawOrder, const Color& baseColor, int32 radius);
		static void DrawDropShadowRect(LinaVG::Drawer* lvg, const Rect& rect, int32 drawOrder, const Color& baseColor, int32 radius);

		static DirectionalLayout* BuildLayoutForPopups(Widget* source);
		static Button*			  BuildIconTextButton(Widget* src, const String& icon, const String& text);

		static bool CheckIfCanShrinkWidgets(const Vector<Widget*>& widgets, float absAmount, bool isX, float minSize);

		static void CorrectPopupPosition(Widget* w);
	};

} // namespace Lina
