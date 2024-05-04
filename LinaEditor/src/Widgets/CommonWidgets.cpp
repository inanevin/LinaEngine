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
#include "Core/GUI/Widgets/Primitives/ShapeRect.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina::Editor
{

	DirectionalLayout* CommonWidgets::BuildWindowButtons(Widget* source)
	{
		DirectionalLayout* layout	 = source->GetWidgetManager()->Allocate<DirectionalLayout>("WindowButtons");
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->GetProps().mode		 = DirectionalLayout::Mode::EqualSizes;

		Button* min = source->GetWidgetManager()->Allocate<Button>("Minimize");
		min->GetText()->SetVisible(false);
		min->GetProps().colorDefaultStart = Theme::GetDef().background0;
		min->GetProps().colorDefaultEnd	  = Theme::GetDef().background0;
		min->GetProps().colorHovered	  = Theme::GetDef().background4;
		min->GetProps().colorPressed	  = Theme::GetDef().background0;
		min->GetProps().rounding		  = 0.0f;
		min->GetProps().outlineThickness  = 0.0f;
		min->GetProps().onClicked		  = [source]() { source->GetWindow()->Minimize(); };
		min->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		min->SetAlignedPosY(0.0f);
		min->SetAlignedSizeY(1.0f);
		layout->AddChild(min);

		Icon* iconMin			 = source->GetWidgetManager()->Allocate<Icon>("Icon");
		iconMin->GetProps().icon = ICON_MINIMIZE;
		iconMin->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		iconMin->SetAlignedPos(Vector2(0.5f, 0.5f));
		iconMin->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		iconMin->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		min->AddChild(iconMin);

		Button* max = source->GetWidgetManager()->Allocate<Button>();
		max->GetText()->SetVisible(false);
		max->GetProps().colorDefaultStart = Theme::GetDef().background0;
		max->GetProps().colorDefaultEnd	  = Theme::GetDef().background0;
		max->GetProps().colorHovered	  = Theme::GetDef().background4;
		max->GetProps().colorPressed	  = Theme::GetDef().background0;
		max->GetProps().rounding		  = 0.0f;
		max->GetProps().outlineThickness  = 0.0f;
		max->GetProps().onClicked		  = [source, max]() {
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

		Icon* iconMax			 = source->GetWidgetManager()->Allocate<Icon>("Icon");
		iconMax->GetProps().icon = ICON_MAXIMIZE;
		iconMax->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		iconMax->SetAlignedPos(Vector2(0.5f, 0.5f));
		iconMax->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		iconMax->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		max->AddChild(iconMax);

		Button* close = source->GetWidgetManager()->Allocate<Button>();
		close->GetText()->SetVisible(false);
		close->GetProps().colorDefaultStart = Theme::GetDef().accentPrimary0;
		close->GetProps().colorDefaultEnd	= Theme::GetDef().accentPrimary0;
		close->GetProps().colorHovered		= Theme::GetDef().accentPrimary2;
		close->GetProps().colorPressed		= Theme::GetDef().accentPrimary1;
		close->GetProps().rounding			= 0.0f;
		close->GetProps().outlineThickness	= 0.0f;
		close->GetProps().onClicked			= [source]() {
			if (source->GetWindow()->GetSID() == LINA_MAIN_SWAPCHAIN)
				Editor::Get()->RequestExit();
			else
				Editor::Get()->CloseWindow(static_cast<StringID>(source->GetWindow()->GetSID()));
		};
		close->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		close->SetAlignedPosY(0.0f);
		close->SetAlignedSizeY(1.0f);
		layout->AddChild(close);

		Icon* iconClose			   = source->GetWidgetManager()->Allocate<Icon>("Icon");
		iconClose->GetProps().icon = ICON_XMARK;
		iconClose->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		iconClose->SetAlignedPos(Vector2(0.5f, 0.5f));
		iconClose->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		iconClose->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		close->AddChild(iconClose);

		return layout;
	}

	DirectionalLayout* CommonWidgets::BuildPopupItemDefault(const String& title, Widget* source, bool disabled, bool hasHeadingIcon, const String& headingIcon, bool hasDropdown, const String& altText)
	{
		DirectionalLayout* item = source->GetWidgetManager()->Allocate<DirectionalLayout>("PopupItemLayout");
		item->GetFlags().Set(WF_USE_FIXED_SIZE_Y | WF_SIZE_ALIGN_X | WF_POS_ALIGN_X);
		item->SetAlignedPosX(0.0f);
		item->SetAlignedSizeX(1.0f);
		item->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		item->GetChildMargins()				  = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		item->GetProps().useHoverColor		  = true;
		item->GetProps().receiveInput		  = true;
		item->GetProps().backgroundStyle	  = DirectionalLayout::BackgroundStyle::Default;
		item->GetProps().colorHovered		  = Theme::GetDef().accentPrimary0;
		item->GetProps().colorBackgroundStart = item->GetProps().colorBackgroundEnd = Color(0, 0, 0, 0);
		item->SetChildPadding(Theme::GetDef().baseIndent);

		if (hasHeadingIcon)
		{
			if (headingIcon.empty())
			{
				Widget* empty = source->GetWidgetManager()->Allocate<Widget>("Empty");
				empty->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_Y);
				empty->SetAlignedPosY(0.0f);
				empty->SetAlignedSizeY(1.0f);
				empty->SetFixedSizeX(Theme::GetDef().baseItemHeight);
				item->AddChild(empty);
			}
			else
			{
				Icon* icon = source->GetWidgetManager()->Allocate<Icon>("Icon");
				icon->GetFlags().Set(WF_POS_ALIGN_Y);
				icon->SetAlignedPosY(0.5f);
				icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
				item->AddChild(icon);
			}
		}

		Text* txt = source->GetWidgetManager()->Allocate<Text>("Text");
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		txt->GetProps().text		  = title;
		txt->GetProps().colorDisabled = Theme::GetDef().silent2;
		item->AddChild(txt);

		if (hasDropdown)
		{
			Icon* dd = source->GetWidgetManager()->Allocate<Icon>("DD");
			dd->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			dd->SetAlignedPos(Vector2(1.0f, 0.5f));
			dd->SetPosAlignmentSourceX(PosAlignmentSource::End);
			dd->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			dd->GetProps().icon = ICON_ARROW_RIGHT;
			item->AddChild(dd);
		}
		if (!altText.empty())
		{
			Text* txt = source->GetWidgetManager()->Allocate<Text>("Text");
			txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			txt->SetAlignedPos(Vector2(1.0f, 0.5f));
			txt->SetPosAlignmentSourceX(PosAlignmentSource::End);
			txt->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			txt->GetProps().text  = altText;
			txt->GetProps().font  = ALT_FONT_BOLD_SID;
			txt->GetProps().color = Theme::GetDef().silent2;
			item->AddChild(txt);
		}

		item->Initialize();
		item->SetIsDisabled(disabled);

		return item;
	}

	DirectionalLayout* CommonWidgets::BuildPopupItemDivider(Widget* source)
	{
		DirectionalLayout* item = source->GetWidgetManager()->Allocate<DirectionalLayout>("Divider");
		item->GetFlags().Set(WF_USE_FIXED_SIZE_Y | WF_SIZE_ALIGN_X | WF_POS_ALIGN_X);
		item->SetAlignedPosX(0.0f);
		item->SetAlignedSizeX(1.0f);
		item->SetFixedSizeY(Theme::GetDef().baseItemHeight * 0.5f);

		ShapeRect* rect = source->GetWidgetManager()->Allocate<ShapeRect>("Shape");
		rect->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		rect->SetAlignedSize(Vector2(1.0f, 0.25f));
		rect->SetAlignedPosY(0.5f);
		rect->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		rect->GetProps().colorStart = rect->GetProps().colorEnd = Theme::GetDef().outlineColorBase;
		item->AddChild(rect);

		return item;
	}

	InfoTooltip* CommonWidgets::ThrowInfoTooltip(const String& str, LogLevel level, float time, Widget* source)
	{
		InfoTooltip* inf			 = source->GetWidgetManager()->Allocate<InfoTooltip>("InfoTooltip");
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
		InfoTooltip* inf				 = manager->Allocate<InfoTooltip>("InfoTooltip");
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
		GenericPopup* pp		  = source->GetWidgetManager()->Allocate<GenericPopup>("GenericPopup");
		pp->GetPopupProps().text  = text;
		pp->GetPopupProps().title = title;
		pp->Initialize();
		return pp;
	}

} // namespace Lina::Editor
