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
#include "Platform/LinaVGIncl.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Resource/Texture.hpp"

namespace Lina::Editor
{
	Vector<Vector<ClipData>> GUIUtility::s_clipStack;

	Vector2 GUIUtility::DrawIcon(int threadID, float dpiScale, const char* icon, const Vector2& centerPos, float scale, Color tint, int drawOrder, float rotation, bool skipCache)
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
		return iconSize;
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

	void GUIUtility::DrawWidgetBackground(int threadID, const Rect& rect, float borderThickness, int drawOrder, bool disabled, bool hasFocus)
	{
		LinaVG::StyleOptions bg;
		bg.color						= LV4((!disabled ? Theme::TC_Dark1 : Theme::TC_Dark25));
		bg.outlineOptions.thickness		= 1.0f;
		bg.outlineOptions.color			= LV4((hasFocus ? Theme::TC_CyanAccent : Theme::TC_Silent1));
		bg.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Outwards;
		LinaVG::DrawRect(threadID, LV2(rect.pos), LV2((rect.pos + rect.size)), bg, 0.0f, drawOrder);
	}

	void GUIUtility::DrawWidgetLabelBox(int threadID, float dpiScale, const char* label, const Rect& rect, int drawOrder, const Color& labelColor)
	{
		LinaVG::StyleOptions bg;
		bg.color = LV4(Theme::TC_Dark2);
		LinaVG::DrawRect(threadID, LV2(rect.pos), LV2((rect.pos + rect.size)), bg, 0.0f, drawOrder);

		LinaVG::TextOptions txtOpts;
		txtOpts.font  = Theme::GetFont(FontType::AltEditor, dpiScale);
		txtOpts.color = LV4(labelColor);

		const Vector2 txtSize = FL2(LinaVG::CalculateTextSize(label, txtOpts));
		const Vector2 txtPos  = Vector2(rect.pos.x + rect.size.x * 0.5f - txtSize.x * 0.5f, rect.pos.y + rect.size.y * 0.5f + txtSize.y * 0.5f);
		LinaVG::DrawTextNormal(threadID, label, LV2(txtPos), txtOpts, 0.0f, drawOrder);
	}

	void GUIUtility::DrawWidgetSliderBox(int threadID, const Rect& rect, int drawOrder)
	{
		LinaVG::StyleOptions bg;
		bg.color = LV4(Theme::TC_Dark2);
		LinaVG::DrawRect(threadID, LV2(rect.pos), LV2((rect.pos + rect.size)), bg, 0.0f, drawOrder);
	}

	void GUIUtility::DrawSheetImage(int threadID, const TextureSheetItem& item, const Vector2& center, const Vector2& size, const Color& tint, int drawOrder)
	{
		LinaVG::DrawImage(threadID, item.texture->GetSID(), LV2(center), LV2(size), LV4(tint), 0.0f, drawOrder, LinaVG::Vec2(1, 1), LinaVG::Vec2(0, 0), LinaVG::Vec2(item.uvTL.x, item.uvTL.y), LinaVG::Vec2(item.uvBR.x, item.uvBR.y));
	}

	Vector2 GUIUtility::DrawTextCentered(int threadID, const char* text, const Rect& rect, LinaVG::TextOptions& opts, int drawOrder)
	{
		const Vector2 textSize = FL2(LinaVG::CalculateTextSize(text, opts));
		const Vector2 textPos  = Vector2(rect.pos.x + rect.size.x * 0.5f - textSize.x * 0.5f, rect.pos.y + rect.size.y * 0.5f + textSize.y * 0.5f);
		LinaVG::DrawTextNormal(threadID, text, LV2(textPos), opts, 0.0f, drawOrder);
		return textSize;
	}

	Vector2 GUIUtility::DrawTextCentered(int threadID, const char* text, const Rect& rect, LinaVG::SDFTextOptions& opts, int drawOrder)
	{
		const Vector2 textSize = FL2(LinaVG::CalculateTextSize(text, opts));
		const Vector2 textPos  = Vector2(rect.pos.x + rect.size.x * 0.5f - textSize.x * 0.5f, rect.pos.y + rect.size.y * 0.5f + textSize.y * 0.5f);
		LinaVG::DrawTextSDF(threadID, text, LV2(textPos), opts, 0.0f, drawOrder);
		return textSize;
	}

	bool GUIUtility::IsInRect(const Vector2& pos, const Rect& rect)
	{
		return pos.x > rect.pos.x && pos.x < rect.pos.x + rect.size.x && pos.y > rect.pos.y && pos.y < rect.pos.y + rect.size.y;
	}
	void GUIUtility::PrepareClipStack(int threadCount)
	{
		s_clipStack.clear();
		s_clipStack.resize(threadCount);
	}

	void GUIUtility::SetClip(int threadID, const Rect& r, const Rect& margin)
	{
		ClipData cd = {
			.clipRect	= r,
			.clipMargin = margin,
		};

		s_clipStack[threadID].push_back(cd);

		LinaVG::SetClipPosX(static_cast<uint32>(r.pos.x + margin.pos.x), threadID);
		LinaVG::SetClipPosY(static_cast<uint32>(r.pos.y + margin.pos.y), threadID);
		LinaVG::SetClipSizeX(static_cast<uint32>(r.size.x + margin.size.x), threadID);
		LinaVG::SetClipSizeY(static_cast<uint32>(r.size.y + margin.size.y), threadID);
	}

	void GUIUtility::UnsetClip(int threadID)
	{
		auto& vec = s_clipStack[threadID];
		vec.pop_back();

		if (vec.empty())
		{
			LinaVG::SetClipPosX(0, threadID);
			LinaVG::SetClipPosY(0, threadID);
			LinaVG::SetClipSizeX(0, threadID);
			LinaVG::SetClipSizeY(0, threadID);
		}
		else
		{
			const ClipData& cd = vec[vec.size() - 1];
			LinaVG::SetClipPosX(static_cast<uint32>(cd.clipRect.pos.x + cd.clipMargin.pos.x), threadID);
			LinaVG::SetClipPosY(static_cast<uint32>(cd.clipRect.pos.y + cd.clipMargin.pos.y), threadID);
			LinaVG::SetClipSizeX(static_cast<uint32>(cd.clipRect.size.x + cd.clipMargin.size.x), threadID);
			LinaVG::SetClipSizeY(static_cast<uint32>(cd.clipRect.size.y + cd.clipMargin.size.y), threadID);
		}
	}
} // namespace Lina::Editor
