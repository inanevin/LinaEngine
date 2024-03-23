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
		min->GetProps().colorDefaultStart	 = Theme::GetDef().background0;
		min->GetProps().colorDefaultEnd		 = Theme::GetDef().background0;
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
		max->GetProps().colorDefaultStart	 = Theme::GetDef().background0;
		max->GetProps().colorDefaultEnd		 = Theme::GetDef().background0;
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

	DirectionalLayout* CommonWidgets::BuildPopupItemDefault(const String& title, Widget* source, bool disabled, bool hasHeadingIcon, const String& headingIcon, bool hasDropdown, const String& altText)
	{
		DirectionalLayout* item = source->Allocate<DirectionalLayout>("PopupItemLayout");
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
				Widget* empty = source->Allocate<Widget>("Empty");
				empty->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_Y);
				empty->SetAlignedPosY(0.0f);
				empty->SetAlignedSizeY(1.0f);
				empty->SetFixedSizeX(Theme::GetDef().baseItemHeight);
				item->AddChild(empty);
			}
			else
			{
				Icon* icon = source->Allocate<Icon>("Icon");
				icon->GetFlags().Set(WF_POS_ALIGN_Y);
				icon->SetAlignedPosY(0.5f);
				icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
				item->AddChild(icon);
			}
		}

		Text* txt = source->Allocate<Text>("Text");
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		txt->GetProps().text		  = title;
		txt->GetProps().colorDisabled = Theme::GetDef().silent2;
		item->AddChild(txt);

        
        if(hasDropdown)
        {
            Icon* dd = source->Allocate<Icon>("DD");
            dd->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
            dd->SetAlignedPos(Vector2(1.0f, 0.5f));
            dd->SetPosAlignmentSourceX(PosAlignmentSource::End);
            dd->SetPosAlignmentSourceY(PosAlignmentSource::Center);
            dd->GetProps().icon = ICON_ARROW_RIGHT;
            item->AddChild(dd);
        }
        if(!altText.empty())
        {
            Text* txt = source->Allocate<Text>("Text");
            txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
            txt->SetAlignedPos(Vector2(1.0f, 0.5f));
            txt->SetPosAlignmentSourceX(PosAlignmentSource::End);
            txt->SetPosAlignmentSourceY(PosAlignmentSource::Center);
            txt->GetProps().text          = altText;
            txt->GetProps().font = ALT_FONT_BOLD_SID;
            txt->GetProps().color = Theme::GetDef().silent2;
            item->AddChild(txt);
        }
        
        item->Initialize();
        item->SetIsDisabled(disabled);

		return item;
	}


	DirectionalLayout* CommonWidgets::BuildPopupItemDivider(Widget* source)
	{
		DirectionalLayout* item = source->Allocate<DirectionalLayout>("Divider");
		item->GetFlags().Set(WF_USE_FIXED_SIZE_Y | WF_SIZE_ALIGN_X | WF_POS_ALIGN_X);
		item->SetAlignedPosX(0.0f);
		item->SetAlignedSizeX(1.0f);
		item->SetFixedSizeY(Theme::GetDef().baseItemHeight * 0.5f);

		ShapeRect* rect = source->Allocate<ShapeRect>("Shape");
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
