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
#include "Core/GUI/Widgets/Layout/Popup.hpp"
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
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace Lina::Editor
{

	DirectionalLayout* CommonWidgets::BuildWindowButtons(Widget* source)
	{
		DirectionalLayout* layout	 = source->GetWidgetManager()->Allocate<DirectionalLayout>("WindowButtons");
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->GetProps().mode		 = DirectionalLayout::Mode::EqualSizes;

		Button* min							   = source->GetWidgetManager()->Allocate<Button>("Minimize");
		min->GetWidgetProps().colorBackground  = Theme::GetDef().background0;
		min->GetWidgetProps().colorHovered	   = Theme::GetDef().background4;
		min->GetWidgetProps().colorPressed	   = Theme::GetDef().background0;
		min->GetWidgetProps().rounding		   = 0.0f;
		min->GetWidgetProps().outlineThickness = 0.0f;
		min->GetProps().onClicked			   = [source]() { source->GetWindow()->Minimize(); };
		min->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		min->SetAlignedPosY(0.0f);
		min->SetAlignedSizeY(1.0f);
		min->SetTooltip(Locale::GetStr(LocaleStr::Minimize));
		min->SetDrawOrderIncrement(1);
		layout->AddChild(min);

		Icon* iconMin			 = source->GetWidgetManager()->Allocate<Icon>("Icon");
		iconMin->GetProps().icon = ICON_MINIMIZE;
		iconMin->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		iconMin->SetAlignedPos(Vector2(0.5f, 0.5f));
		iconMin->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		iconMin->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		min->AddChild(iconMin);

		Button* max							   = source->GetWidgetManager()->Allocate<Button>();
		max->GetWidgetProps().colorBackground  = Theme::GetDef().background0;
		max->GetWidgetProps().colorHovered	   = Theme::GetDef().background4;
		max->GetWidgetProps().colorPressed	   = Theme::GetDef().background0;
		max->GetWidgetProps().rounding		   = 0.0f;
		max->GetWidgetProps().outlineThickness = 0.0f;
		max->SetDrawOrderIncrement(1);

		max->GetProps().onClicked = [source, max]() {
			if (source->GetWindow()->GetIsMaximized())
				source->GetWindow()->Restore();
			else
				source->GetWindow()->Maximize();

			max->GetText()->GetProps().text = source->GetWindow()->GetIsMaximized() ? ICON_RESTORE : ICON_MAXIMIZE;
			max->GetText()->CalculateTextSize();
		};
		max->SetTooltip(Locale::GetStr(LocaleStr::MaximizeRestore));
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

		Button* close							 = source->GetWidgetManager()->Allocate<Button>();
		close->GetWidgetProps().colorBackground	 = Theme::GetDef().accentPrimary0;
		close->GetWidgetProps().colorHovered	 = Theme::GetDef().accentPrimary2;
		close->GetWidgetProps().colorPressed	 = Theme::GetDef().accentPrimary1;
		close->GetWidgetProps().rounding		 = 0.0f;
		close->GetWidgetProps().outlineThickness = 0.0f;
		close->SetDrawOrderIncrement(1);
		close->SetTooltip(Locale::GetStr(LocaleStr::Close));
		close->GetProps().onClicked = [source]() {
			if (source->GetWindow()->GetSID() == LINA_MAIN_SWAPCHAIN)
				Editor::Get()->RequestExit();
			else
				Editor::Get()->GetWindowPanelManager().CloseWindow(static_cast<StringID>(source->GetWindow()->GetSID()));
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

	Widget* CommonWidgets::GetPopupItemWithSelectionToggle(Widget* source, const String& title, bool isSelected)
	{
		WidgetManager*	   wm		 = source->GetWidgetManager();
		DirectionalLayout* layout	 = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_SIZE_X_TOTAL_CHILDREN);
		layout->SetAlignedPosX(0.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->SetChildPadding(Theme::GetDef().baseIndent);
		layout->GetChildMargins()			= {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		layout->GetBorderThickness().bottom = Theme::GetDef().baseOutlineThickness;
		layout->GetBorderColor()			= Theme::GetDef().background1;

		Icon* icon			  = wm->Allocate<Icon>("IconBG");
		icon->GetProps().icon = ICON_CIRCLE_FILLED;
		icon->GetFlags().Set(WF_POS_ALIGN_Y);
		icon->SetAlignedPosY(0.5f);
		icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		layout->AddChild(icon);

		Text* txt			 = wm->Allocate<Text>("Text");
		txt->GetProps().text = title;
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		layout->AddChild(txt);

		return layout;
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
