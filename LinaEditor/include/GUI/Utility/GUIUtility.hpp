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

#ifndef GUIUtility_HPP
#define GUIUtility_HPP

#include "Core/SizeDefinitions.hpp"
#include "Core/Theme.hpp"
#include "Data/String.hpp"

namespace Lina
{
	class Vector2;
	class Rect;
	struct TextureSheetItem;

} // namespace Lina

namespace LinaVG
{
	struct TextOptions;
	struct SDFTextOptions;
} // namespace LinaVG

namespace Lina::Editor
{

	class GUIUtility
	{
	public:
		static void DrawIcon(int threadID, float dpiScale, const char* icon, const Vector2& centerPos, float scale, Color tint = Color::White, int drawOrder = 0, float rotation = 0.0f, bool skipCache = false);
		static void DrawTitleBackground(int threadID, const Rect& rect, int drawOrder);
		static void DrawDockBackground(int threadID, const Rect& rect, int drawOrder);
		static void DrawPanelBackground(int threadID, const Rect& rect, int drawOrder);
		static void DrawPopupBackground(int threadID, const Rect& rect, float borderThickness, int drawOrder);
		static void DrawSheetImage(int threadID, const TextureSheetItem& item, const Vector2& center, const Vector2& size, const Color& tint, int drawOrder);

		static Vector2 DrawTextCentered(int threadID, const char* text, const Rect& rect, LinaVG::TextOptions& opts, int drawOrder);
		static Vector2 DrawTextCentered(int threadID, const char* text, const Rect& rect, LinaVG::SDFTextOptions& opts, int drawOrder);

		static bool IsInRect(const Vector2& pos, const Rect& rect);
	};
} // namespace Lina::Editor

#endif
