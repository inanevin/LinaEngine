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

#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Popups/InfoTooltip.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/System.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina::Editor
{
	DirectionalLayout* CommonWidgets::BuildWindowBar(const String& title, bool hasWindowButtons, bool hasIcon, Widget* source)
	{
		DirectionalLayout* layout = source->Allocate<DirectionalLayout>("WindowBar");
		layout->SetChildPadding(Theme::GetDef().baseIndent);

		if (hasIcon)
		{
			Icon* icon			  = source->Allocate<Icon>("WindowBarIcon");
			icon->GetProps().icon = ICON_LINA_LOGO;
			icon->GetFlags().Set(WF_POS_ALIGN_Y);
			icon->SetAlignedPosY(0.5f);
			icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			layout->AddChild(icon);
		}

		Text* text			  = source->Allocate<Text>("WindowBarTitle");
		text->GetProps().text = title;
		text->GetFlags().Set(WF_POS_ALIGN_Y);
		text->SetAlignedPosY(0.5f);
		text->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		layout->AddChild(text);

		if (hasWindowButtons)
		{
			DirectionalLayout* wb = CommonWidgets::BuildWindowButtons(source);
			wb->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
			wb->SetAlignedPosY(0.0f);
			wb->SetAlignedSizeY(1.0f);
			wb->SetPosAlignmentSourceX(PosAlignmentSource::End);
			wb->SetFixedSizeX(Theme::GetDef().baseItemHeight * 6.0f);
			layout->AddChild(wb);
		}

		return layout;
	}

	DirectionalLayout* CommonWidgets::BuildWindowButtons(Widget* source)
	{
		DirectionalLayout* layout	 = source->Allocate<DirectionalLayout>("WindowButtons");
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->GetProps().mode		 = DirectionalLayout::Mode::EqualSizes;

		Button* min							 = source->Allocate<Button>("Minimize");
		min->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
		min->GetText()->GetProps().text		 = ICON_MINIMIZE;
		min->GetText()->GetProps().textScale = 0.5f;
		min->GetProps().colorDefaultStart	 = Theme::GetDef().black;
		min->GetProps().colorDefaultEnd		 = Theme::GetDef().black;
		min->GetProps().colorHovered		 = Theme::GetDef().background4;
		min->GetProps().colorPressed		 = Theme::GetDef().background0;
		min->GetProps().rounding			 = 0.0f;
		min->GetProps().outlineThickness	 = 0.0f;
		min->GetProps().onClicked			 = [source]() { source->GetWindow()->Minimize(); };
		min->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		min->SetAlignedPosY(0.0f);
		min->SetAlignedSizeY(1.0f);
		layout->AddChild(min);

		Button* max							 = source->Allocate<Button>();
		max->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
		max->GetText()->GetProps().text		 = source->GetWindow()->GetIsMaximized() ? ICON_RESTORE : ICON_MAXIMIZE;
		max->GetText()->GetProps().textScale = 0.5f;
		max->GetProps().colorDefaultStart	 = Theme::GetDef().black;
		max->GetProps().colorDefaultEnd		 = Theme::GetDef().black;
		max->GetProps().colorHovered		 = Theme::GetDef().background4;
		max->GetProps().colorPressed		 = Theme::GetDef().background0;
		max->GetProps().rounding			 = 0.0f;
		max->GetProps().outlineThickness	 = 0.0f;
		max->GetProps().onClicked			 = [source, max]() {
			   if (source->GetWindow()->GetIsMaximized())
				   source->GetWindow()->Restore();
			   else
				   source->GetWindow()->Maximize();

			   max->GetText()->GetProps().text = source->GetWindow()->GetIsMaximized() ? ICON_RESTORE : ICON_MAXIMIZE;
			   max->GetText()->CalculateTextSize();
		};
		max->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		max->SetAlignedPosY(0.0f);
		max->SetAlignedSizeY(1.0f);
		layout->AddChild(max);

		Button* close						   = source->Allocate<Button>();
		close->GetText()->GetProps().font	   = Theme::GetDef().iconFont;
		close->GetText()->GetProps().text	   = ICON_XMARK;
		close->GetText()->GetProps().textScale = 0.5f;
		close->GetProps().colorDefaultStart	   = Theme::GetDef().accentPrimary0;
		close->GetProps().colorDefaultEnd	   = Theme::GetDef().accentPrimary0;
		close->GetProps().colorHovered		   = Theme::GetDef().accentPrimary2;
		close->GetProps().colorPressed		   = Theme::GetDef().accentPrimary1;
		close->GetProps().rounding			   = 0.0f;
		close->GetProps().outlineThickness	   = 0.0f;
		close->GetProps().onClicked			   = [source]() {
			   if (source->GetWindow()->GetSID() == LINA_MAIN_SWAPCHAIN)
				   source->GetSystem()->CastSubsystem<Editor>(SubsystemType::Editor)->RequestExit();
			   else
				   source->GetWindow()->Close();
		};
		close->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		close->SetAlignedPosY(0.0f);
		close->SetAlignedSizeY(1.0f);
		layout->AddChild(close);

		return layout;
	}

	void CommonWidgets::DrawGradLine(int32 threadIndex, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& baseColor)
	{
		Color weak = baseColor, strong = baseColor;
		strong.w = 1.0f;
		weak.w	 = 0.0f;

		LinaVG::StyleOptions style;
		style.color.start = strong.AsLVG4();
		style.color.end	  = weak.AsLVG4();
		LinaVG::DrawRect(threadIndex, start.AsLVG(), end.AsLVG(), style, 0.0f, drawOrder);
	}

	void CommonWidgets::DrawGradLineCentral(int32 threadIndex, const Vector2& start, const Vector2& end, int32 drawOrder, const Color& baseColor)
	{
		Color weak = baseColor, strong = baseColor;
		strong.w = 1.0f;
		weak.w	 = 0.0f;

		LinaVG::StyleOptions style;
		style.color.start = weak.AsLVG4();
		style.color.end	  = strong.AsLVG4();

		LinaVG::StyleOptions style2;
		style2.color.start = strong.AsLVG4();
		style2.color.end   = weak.AsLVG4();

		LinaVG::DrawRect(threadIndex, start.AsLVG(), Vector2((end.x + start.x) * 0.5f, end.y).AsLVG(), style, 0.0f, drawOrder);
		LinaVG::DrawRect(threadIndex, Vector2((end.x + start.x) * 0.5f, start.y).AsLVG(), Vector2(end.x, end.y).AsLVG(), style2, 0.0f, drawOrder);
	}

	InfoTooltip* CommonWidgets::ThrowInfoTooltip(const String& str, LogLevel level, float time, Widget* source)
	{
		InfoTooltip* inf			 = source->Allocate<InfoTooltip>("InfoTooltip");
		inf->GetTooltipProps().text	 = str;
		inf->GetTooltipProps().level = level;
		inf->GetTooltipProps().time	 = time;
		inf->GetFlags().Set(WF_USE_FIXED_SIZE_Y | WF_CONTROLS_DRAW_ORDER);
		inf->SetDrawOrder(FOREGROUND_HIGHP_DRAW_ORDER);
		inf->SetFixedSize(Theme::GetDef().baseItemHeight);
		inf->Initialize();
		source->GetWidgetManager()->AddToForeground(inf);

		LinaGX::Window* window	   = source->GetWindow();
		const Vector2	windowSize = Vector2(static_cast<float>(window->GetSize().x), static_cast<float>(window->GetSize().y));

		const Vector2 sz = inf->GetSize();

		// Try right
		inf->SetPos(Vector2(source->GetRect().GetEnd().x, source->GetPosY()));
		if (inf->GetRect().GetEnd().x < windowSize.x)
		{
			inf->GetTooltipProps().direction = Direction::Right;
			return inf;
		}

		// Left
		inf->SetPos(Vector2(source->GetPos().x - inf->GetSizeX(), source->GetPosY()));
		if (inf->GetRect().pos.x > 0.0f)
		{
			inf->GetTooltipProps().direction = Direction::Left;
			return inf;
		}

		// Top
		inf->SetPos(source->GetPos() - Vector2(0.0f, inf->GetSizeY()));
		if (inf->GetPosY() > 0.0f)
		{
			inf->GetTooltipProps().direction = Direction::Top;
			return inf;
		}

		// Last resort bottom
		inf->SetPos(Vector2(source->GetPosX(), source->GetRect().GetEnd().y));
		inf->GetTooltipProps().direction = Direction::Bottom;

		return inf;
	}

	InfoTooltip* CommonWidgets::ThrowInfoTooltip(const String& str, LogLevel level, float time, WidgetManager* manager, const Vector2& targetPos)
	{
		InfoTooltip* inf				 = manager->GetForegroundRoot()->Allocate<InfoTooltip>("InfoTooltip");
		inf->GetTooltipProps().text		 = str;
		inf->GetTooltipProps().level	 = level;
		inf->GetTooltipProps().time		 = time;
		inf->GetTooltipProps().direction = Direction::Center;
		inf->SetPos(targetPos);
		manager->AddToForeground(inf);
		return inf;
	}

	float CommonWidgets::GetPopupWidth(LinaGX::Window* window)
	{
		const float itemHeight = Theme::GetDef().baseItemHeight;
		return Math::Clamp(window->GetMonitorSize().x * 0.3f, 200.0f, 600.0f);
	}

	GenericPopup* CommonWidgets::ThrowGenericPopup(const String& title, const String& text, Widget* source)
	{
		GenericPopup* pp		  = source->Allocate<GenericPopup>("GenericPopup");
		pp->GetPopupProps().text  = text;
		pp->GetPopupProps().title = title;
		pp->Initialize();
		source->GetWidgetManager()->AddToForeground(pp);
		return pp;
	}
} // namespace Lina::Editor
