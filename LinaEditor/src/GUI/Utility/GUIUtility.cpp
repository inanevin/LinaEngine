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

#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Resource/Texture.hpp"

using namespace Lina;

std::string from_u8string(const std::string& s)
{
	return s;
}
std::string from_u8string(std::string&& s)
{
	return std::move(s);
}
#if defined(__cpp_lib_char8_t)
std::string from_u8string(const std::u8string& s)
{
	return std::string(s.begin(), s.end());
}
#endif

namespace Lina::Editor
{
	void GUIUtility::DrawIcon(int threadID, float dpiScale, const char* icon, const Vector2& centerPos, float scale, Color tint, int drawOrder, float rotation, bool skipCache)
	{
		LinaVG::SDFTextOptions opts;
		opts.font		  = Theme::GetFont(FontType::EditorIcons, dpiScale);
		opts.color		  = LV4(tint);
		opts.sdfThickness = 0.5f;
		opts.sdfSoftness  = 0.5f;
		opts.textScale	  = scale;

		const Vector2 iconSize = FL2(LinaVG::CalculateTextSize(icon, opts));
		const Vector2 pos	   = Vector2(centerPos.x - iconSize.x * 0.5f, centerPos.y + iconSize.y * 0.5f * opts.sdfThickness + 1.0f);
		LinaVG::DrawTextSDF(threadID, icon, LV2(pos), opts, rotation, drawOrder, skipCache);
	}

	void GUIUtility::DrawTitleBackground(int threadID, const Rect& rect, int drawOrder)
	{
		LinaVG::StyleOptions bg;
		bg.color = LV4(Theme::TC_Dark1);
		LinaVG::DrawRect(threadID, LV2(rect.pos), LV2((rect.pos + rect.size)), bg, 0.0f, drawOrder);
	}

	void GUIUtility::DrawDockBackground(int threadID, const Rect& rect, int drawOrder)
	{
		LinaVG::StyleOptions bg;
		bg.color = LV4(Theme::TC_Dark2);
		LinaVG::DrawRect(threadID, LV2(rect.pos), LV2((rect.pos + rect.size)), bg, 0.0f, drawOrder);
	}

	void GUIUtility::DrawPanelBackground(int threadID, const Rect& rect, int drawOrder)
	{
		LinaVG::StyleOptions bg;
		bg.color = LV4(Theme::TC_Dark3);
		LinaVG::DrawRect(threadID, LV2(rect.pos), LV2((rect.pos + rect.size)), bg, 0.0f, drawOrder);
	}

	void GUIUtility::DrawPopupBackground(int threadID, const Rect& rect, float borderThickness, int drawOrder)
	{
		LinaVG::StyleOptions bg;
		bg.color						= LV4(Theme::TC_Dark1);
		bg.outlineOptions.thickness		= borderThickness;
		bg.outlineOptions.color			= LV4(Theme::TC_Silent0);
		bg.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
		LinaVG::DrawRect(threadID, LV2(rect.pos), LV2((rect.pos + rect.size)), bg, 0.0f, drawOrder);
	}

	void GUIUtility::DrawSheetImage(int threadID, const TextureSheetItem& item, const Vector2& center, const Vector2& size, const Color& tint, int drawOrder)
	{
		LinaVG::DrawImage(threadID, item.texture->GetSID(), LV2(center), LV2(size), LV4(tint), 0.0f, drawOrder, LinaVG::Vec2(1, 1), LinaVG::Vec2(0, 0), LinaVG::Vec2(item.uvTL.x, item.uvTL.y), LinaVG::Vec2(item.uvBR.x, item.uvBR.y));
	}

	bool GUIUtility::IsInRect(const Vector2& pos, const Rect& rect)
	{
		return pos.x > rect.pos.x && pos.x < rect.pos.x + rect.size.x && pos.y > rect.pos.y && pos.y < rect.pos.y + rect.size.y;
	}
} // namespace Lina::Editor
