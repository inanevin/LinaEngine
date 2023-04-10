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

#include "GUI/Nodes/Widgets/GUINodeTab.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/Editor.hpp"
#include "GUI/Nodes/Widgets/GUINodeButton.hpp"
#include "Math/Math.hpp"
#include "Core/SystemInfo.hpp"

namespace Lina::Editor
{
#define OFFSET_FROM_END 0.9f
#define CLOSEBUT_SPEED	25.0f

	GUINodeTab::GUINodeTab(Editor* editor, ISwapchain* swapchain, int drawOrder) : GUINode(editor, swapchain, drawOrder)
	{
		m_closeButton = new GUINodeButton(editor, swapchain, drawOrder);
		m_closeButton->SetIsIcon(true)->SetFitType(ButtonFitType::None)->SetText(TI_CROSS);
		m_closeButton->SetDefaultColor(Theme::TC_Light1)->SetHoveredColor(Theme::TC_RedAccent)->SetPressedColor(Theme::TC_Dark3);
		m_closeButton->SetTextScale(0.5f);
		AddChildren(m_closeButton);
	}
	void GUINodeTab::Draw(int threadID)
	{
		if (!m_visible)
			return;

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());

		Vector<LinaVG::Vec2> points;

		points.push_back(LV2(m_rect.pos));
		points.push_back(LV2(Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END, m_rect.pos.y)));
		points.push_back(LV2((m_rect.pos + m_rect.size)));
		points.push_back(LV2(Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y)));

		LinaVG::StyleOptions opts;
		opts.color	   = LV4(Theme::TC_Light1);
		opts.aaEnabled = false;

		LinaVG::DrawConvex(threadID, points.data(), static_cast<int>(points.size()), opts, 0.0f, m_drawOrder);

		LinaVG::TextOptions textOpts;
		textOpts.font		   = Theme::GetFont(FontType::DefaultEditor, m_swapchain->GetWindowDPIScale());
		const Vector2 textSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), textOpts));
		const Vector2 textPos  = Vector2(m_rect.pos.x + padding, m_rect.pos.y + m_rect.size.y * 0.5f + textSize.y * 0.5f);
		LinaVG::DrawTextNormal(threadID, m_title.c_str(), LV2(textPos), textOpts, 0.0f, m_drawOrder);

		const Vector2 closeButtonSize	= textSize.y;
		const Vector2 closeButtonCenter = textPos + Vector2(textSize.x + padding * 0.5f, 0.0f);
		// m_closeButton->SetRect(Rect(Vector2(textPos.x + textSize.x + padding, textPos.y - textSize.y), closeButtonSize));
		// m_closeButton->Draw(threadID);

		if (m_isHovered)
		{
			const Rect closeRect = Rect(Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END - padding * 1.0f, m_rect.pos.y), Vector2(m_rect.size.x * (1.0f - OFFSET_FROM_END) + padding * 1.5f, m_rect.size.y));

			if (GUIUtility::IsInRect(m_swapchain->GetMousePos(), closeRect))
				m_closeButtonAnimationAlpha = Math::Lerp(m_closeButtonAnimationAlpha, 1.0f, SystemInfo::GetDeltaTime() * CLOSEBUT_SPEED);
			else
				m_closeButtonAnimationAlpha = Math::Lerp(m_closeButtonAnimationAlpha, 0.0f, SystemInfo::GetDeltaTime() * CLOSEBUT_SPEED);
		}
		else
			m_closeButtonAnimationAlpha = Math::Lerp(m_closeButtonAnimationAlpha, 0.0f, SystemInfo::GetDeltaTime() * CLOSEBUT_SPEED);

		DrawCloseButton(threadID, m_closeButtonAnimationAlpha);

		// GUIUtility::DrawSheetImage(threadID, m_editor->GetEditorImage(EDITOR_IMAGE_CROSS), closeButtonCenter, closeButtonSize, Color::White, m_drawOrder);
	}

	Vector2 GUINodeTab::CalculateTabSize()
	{
		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());
		float		totalX	= 0.0f;
		totalX += padding;

		LinaVG::TextOptions opts;
		opts.font			   = Theme::GetFont(FontType::DefaultEditor, m_swapchain->GetWindowDPIScale());
		const Vector2 textSize = FL2(LinaVG::CalculateTextSize(m_title.c_str(), opts));
		totalX += textSize.x + padding;

		const float closeButtonSize = textSize.y;
		totalX += closeButtonSize + padding;

		return Vector2(totalX, textSize.y + padding);
	}

	void GUINodeTab::DrawCloseButton(int threadID, float t)
	{
		if (t < 0.05f)
			return;

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());

		const float	  offset	 = padding * 1.5f;
		const float	  yOffset	 = 1.0f;
		const Vector2 originalTL = Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END, m_rect.pos.y + yOffset);
		const Vector2 originalBL = Vector2(m_rect.pos.x + m_rect.size.x, m_rect.pos.y + m_rect.size.y);
		const Vector2 targetTL	 = Vector2(originalTL.x - offset, m_rect.pos.y + yOffset);
		const Vector2 targetBL	 = Vector2(originalBL.x - offset, m_rect.pos.y + m_rect.size.y - yOffset);
		const Vector2 p0		 = Math::Lerp(originalTL, targetTL, t);
		const Vector2 p3		 = Math::Lerp(originalBL, targetBL, t);
		const Vector2 p1		 = Vector2(m_rect.pos.x + m_rect.size.x * OFFSET_FROM_END, m_rect.pos.y + yOffset);
		const Vector2 p2		 = m_rect.pos + m_rect.size - Vector2(0, yOffset);

		Vector<LinaVG::Vec2> points;

		points.push_back(LV2(p0));
		points.push_back(LV2(p1));
		points.push_back(LV2(p2));
		points.push_back(LV2(p3));

		LinaVG::StyleOptions opts;
		Color				 bg = Theme::TC_RedAccent;
		bg.w					= Math::Lerp(0.0f, 1.0f, t);
		opts.color				= LV4(bg);
		opts.aaEnabled			= true;
		opts.aaMultiplier		= 1.0f;

		LinaVG::DrawConvex(threadID, points.data(), static_cast<int>(points.size()), opts, 0.0f, m_drawOrder + 1);

		const Vector2 iconCenter = (p2 + p0) * 0.5f;

		Color iconColor = t < 0.7f ? Color(0.0f, 0.0f, 0.0f, 0.0f) : Math::Lerp(Color(0, 0, 0, 0), Color::White, t * 10.0f);
		iconColor.w		= Math::Clamp(iconColor.w, 0.0f, 1.0f);
		GUIUtility::DrawIcon(threadID, m_swapchain->GetWindowDPIScale(), TI_CROSS, iconCenter, 1.0f, iconColor, m_drawOrder + 2, 90.0f * t, true);
	}

} // namespace Lina::Editor
