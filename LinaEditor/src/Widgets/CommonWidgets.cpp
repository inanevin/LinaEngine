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
#include "Editor/Widgets/FX/LinaLoading.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Editor/Widgets/Compound/ColorWheelCompound.hpp"
#include "Editor/Widgets/Panel/PanelColorWheel.hpp"
#include "Editor/Widgets/Compound/ResourceDirectoryBrowser.hpp"
#include "Editor/Widgets/Compound/EntityBrowser.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Widgets/FX/ProgressCircleFill.hpp"
#include "Editor/Widgets/Panel/PanelGenericSelector.hpp"
#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Common/Data/HashSet.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"

#include "Core/Meta/ProjectData.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Checkbox.hpp"
#include "Core/GUI/Widgets/Primitives/ColorField.hpp"
#include "Core/GUI/Widgets/Primitives/ColorSlider.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Core/Resources/ResourceDirectory.hpp"
#include "Core/Application.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/EntityWorld.hpp"

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
		min->GetWidgetProps().tooltip			 = Locale::GetStr(LocaleStr::Minimize);
		min->GetWidgetProps().drawOrderIncrement = 1;
		layout->AddChild(min);

		Icon* iconMin			 = source->GetWidgetManager()->Allocate<Icon>("Icon");
		iconMin->GetProps().icon = ICON_MINIMIZE;
		iconMin->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		iconMin->SetAlignedPos(Vector2(0.5f, 0.5f));
		iconMin->SetAnchorX(Anchor::Center);
		iconMin->SetAnchorY(Anchor::Center);
		iconMin->GetProps().dynamicSizeScale	= 0.7f;
		iconMin->GetProps().dynamicSizeToParent = true;
		min->AddChild(iconMin);

		Button* max								 = source->GetWidgetManager()->Allocate<Button>();
		max->GetWidgetProps().colorBackground	 = Theme::GetDef().background0;
		max->GetWidgetProps().colorHovered		 = Theme::GetDef().background4;
		max->GetWidgetProps().colorPressed		 = Theme::GetDef().background0;
		max->GetWidgetProps().rounding			 = 0.0f;
		max->GetWidgetProps().outlineThickness	 = 0.0f;
		max->GetWidgetProps().drawOrderIncrement = 1;

		max->GetProps().onClicked = [source, max]() {
			if (source->GetWindow()->GetIsMaximized())
				source->GetWindow()->Restore();
			else
				source->GetWindow()->Maximize();

			max->GetText()->GetProps().text = source->GetWindow()->GetIsMaximized() ? ICON_RESTORE : ICON_MAXIMIZE;
			max->GetText()->CalculateTextSize();
		};
		max->GetWidgetProps().tooltip = Locale::GetStr(LocaleStr::MaximizeRestore);
		max->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		max->SetAlignedPosY(0.0f);
		max->SetAlignedSizeY(1.0f);
		layout->AddChild(max);

		Icon* iconMax			 = source->GetWidgetManager()->Allocate<Icon>("Icon");
		iconMax->GetProps().icon = ICON_MAXIMIZE;
		iconMax->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		iconMax->SetAlignedPos(Vector2(0.5f, 0.5f));
		iconMax->SetAnchorX(Anchor::Center);
		iconMax->SetAnchorY(Anchor::Center);
		iconMax->GetProps().dynamicSizeScale	= 0.7f;
		iconMax->GetProps().dynamicSizeToParent = true;
		max->AddChild(iconMax);

		Button* close							   = source->GetWidgetManager()->Allocate<Button>();
		close->GetWidgetProps().colorBackground	   = Theme::GetDef().accentPrimary0;
		close->GetWidgetProps().colorHovered	   = Theme::GetDef().accentPrimary2;
		close->GetWidgetProps().colorPressed	   = Theme::GetDef().accentPrimary1;
		close->GetWidgetProps().rounding		   = 0.0f;
		close->GetWidgetProps().outlineThickness   = 0.0f;
		close->GetWidgetProps().drawOrderIncrement = 1;
		close->GetWidgetProps().tooltip			   = Locale::GetStr(LocaleStr::Close);
		close->GetProps().onClicked				   = [source]() {
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
		iconClose->SetAnchorX(Anchor::Center);
		iconClose->SetAnchorY(Anchor::Center);
		iconClose->GetProps().dynamicSizeScale	  = 0.7f;
		iconClose->GetProps().dynamicSizeToParent = true;
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
		source->GetWidgetManager()->AddToForeground(inf);
		inf->Initialize();

		const Vector2 windowSize = source->GetWidgetManager()->GetSize();
		const Vector2 sz		 = inf->GetSize();

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
		layout->GetWidgetProps().childPadding			= Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins			= {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		layout->GetWidgetProps().borderThickness.bottom = Theme::GetDef().baseOutlineThickness;
		layout->GetWidgetProps().colorBorders			= Theme::GetDef().background1;

		Icon* icon			  = wm->Allocate<Icon>("IconBG");
		icon->GetProps().icon = ICON_CIRCLE_FILLED;
		icon->GetFlags().Set(WF_POS_ALIGN_Y);
		icon->SetAlignedPosY(0.5f);
		icon->SetAnchorY(Anchor::Center);
		layout->AddChild(icon);

		Text* txt			 = wm->Allocate<Text>("Text");
		txt->GetProps().text = title;
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		layout->AddChild(txt);

		return layout;
	}

	Widget* CommonWidgets::BuildPayloadForPanel(Widget* src, const String& name)
	{
		WidgetManager*	   wm				  = src->GetWidgetManager();
		DirectionalLayout* layout			  = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_SIZE_X_TOTAL_CHILDREN | WF_USE_FIXED_SIZE_Y);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);

		Icon* icon			   = wm->Allocate<Icon>("IconBG");
		icon->GetProps().icon  = ICON_LINA_LOGO;
		icon->GetProps().color = Theme::GetDef().accentPrimary0;
		icon->GetFlags().Set(WF_POS_ALIGN_Y);
		icon->SetAlignedPosY(0.5f);
		icon->SetAnchorY(Anchor::Center);
		layout->AddChild(icon);

		Text* txt			 = wm->Allocate<Text>("Text");
		txt->GetProps().text = name;
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		layout->AddChild(txt);
		layout->Initialize();
		return layout;
	}

	FoldLayout* CommonWidgets::BuildTreeItem(Widget* src, const ResDirItemProperties& props)
	{
		WidgetManager* wm = src->GetWidgetManager();

		bool* unfoldValue = props.unfoldValue;

		FoldLayout* fold = wm->Allocate<FoldLayout>("Fold");
		fold->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
		fold->SetAlignedPosX(0.0f);
		fold->SetAlignedSizeX(1.0f);
		fold->GetProps().tweenDuration = 0.25f;
		fold->GetProps().tweenPower	   = Theme::GetDef().baseIndentInner;
		fold->SetIsUnfolded(unfoldValue != nullptr ? *unfoldValue : false);
		fold->SetUserData(props.userData);
		fold->GetProps().onFoldChanged = [fold, unfoldValue](bool unfolded) {
			if (unfoldValue)
				*unfoldValue = unfolded;

			Icon* icon = fold->GetWidgetOfType<Icon>(fold);
			if (icon)
			{
				icon->GetProps().useAltIcon = unfolded;
				icon->CalculateIconSize();
			}
		};

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_TREEITEM);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);

		layout->GetWidgetProps().childPadding			  = Theme::GetDef().baseIndentInner;
		layout->GetWidgetProps().childMargins.left		  = props.margin;
		layout->GetWidgetProps().childMargins.right		  = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().outlineThickness		  = 0.0f;
		layout->GetWidgetProps().rounding				  = 0.0f;
		layout->GetWidgetProps().drawBackground			  = true;
		layout->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Vertical;
		layout->GetWidgetProps().colorBackground		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		// layout->GetWidgetProps().interpolateColor		  = true;
		layout->GetWidgetProps().colorInterpolateSpeed = 20.0f;
		layout->SetUserData(props.userData);
		fold->AddChild(layout);

		Widget* space0 = wm->Allocate<Widget>("Space0");
		space0->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
		space0->SetAlignedPosY(0.5f);
		space0->SetAnchorY(Anchor::Center);
		space0->SetAlignedSize(Vector2(0.55f, 1.0f));
		layout->AddChild(space0);

		Widget* space1 = wm->Allocate<Widget>("Space1");
		space1->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
		space1->SetAlignedPosY(0.5f);
		space1->SetAnchorY(Anchor::Center);
		space1->SetAlignedSize(Vector2(1.0f, 0.75f));
		layout->AddChild(space1);

		if (!props.chevron.empty())
		{
			Icon* icon					= wm->Allocate<Icon>("Chevron");
			icon->GetProps().icon		= props.chevron;
			icon->GetProps().iconAlt	= props.chevronAlt;
			icon->GetProps().useAltIcon = unfoldValue ? *unfoldValue : false;
			icon->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
			icon->SetAlignedPos(Vector2(0.5f, 0.5f));
			icon->SetAnchorY(Anchor::Center);
			icon->SetAnchorX(Anchor::Center);
			icon->GetProps().dynamicSizeToParent = true;
			icon->GetProps().dynamicSizeScale	 = 0.9f;
			space0->AddChild(icon);
		}
		else if (!props.typeText.empty())
		{
			Text* text				   = wm->Allocate<Text>("TypeText");
			text->GetProps().textScale = 0.9f;
			text->UpdateTextAndCalcSize(props.typeText);
			text->GetProps().color = props.typeColor;
			text->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
			text->SetAlignedPos(Vector2(0.5f, 0.5f));
			text->SetAnchorY(Anchor::Center);
			text->SetAnchorX(Anchor::Center);
			space0->AddChild(text);
		}

		if (!props.mainIcon.empty())
		{
			Icon* icon			   = wm->Allocate<Icon>("MainIcon");
			icon->GetProps().icon  = props.mainIcon;
			icon->GetProps().color = props.mainIconColor;
			icon->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
			icon->SetAlignedPos(Vector2(0.5f, 0.5f));
			icon->SetAnchorY(Anchor::Center);
			icon->SetAnchorX(Anchor::Center);
			icon->GetProps().dynamicSizeToParent = true;
			icon->GetProps().dynamicSizeScale	 = 0.9f;
			space1->AddChild(icon);
		}
		else if (props.image != nullptr)
		{
			space1->GetWidgetProps().drawBackground	  = true;
			space1->GetWidgetProps().textureAtlas	  = props.image;
			space1->GetWidgetProps().outlineThickness = 0.0f;
			space1->GetWidgetProps().rounding		  = 0.0f;
			space1->GetWidgetProps().colorBackground  = Color::White;
			space1->SetCustomTooltipUserData(space1);
			space1->SetBuildCustomTooltip(BIND(&CommonWidgets::BuildThumbnailTooltip, std::placeholders::_1));
		}

		if (!props.title.empty())
		{
			Text* text = wm->Allocate<Text>("Title");
			text->UpdateTextAndCalcSize(props.title);
			text->GetProps().color = Theme::GetDef().foreground0;
			text->GetFlags().Set(WF_POS_ALIGN_Y);
			text->SetAlignedPosY(0.5f);
			text->SetAnchorY(Anchor::Center);
			layout->AddChild(text);
		}

		if (!props.footerIcon.empty())
		{
			Icon* footer			= wm->Allocate<Icon>("Footer");
			footer->GetProps().icon = props.footerIcon;
			footer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			footer->SetAlignedPosY(0.5f);
			footer->SetAlignedPosX(1.0f);
			footer->SetAnchorX(Anchor::End);
			footer->SetAnchorY(Anchor::Center);
			footer->GetProps().dynamicSizeToParent = true;
			footer->GetProps().dynamicSizeScale	   = 0.8f;
			footer->GetProps().color			   = props.footerIconColor;
			layout->AddChild(footer);
		}

		fold->Initialize();
		return fold;
	}

	FoldLayout* CommonWidgets::BuildEntityItem(Widget* src, const EntityItemProperties& props)
	{
		WidgetManager* wm = src->GetWidgetManager();

		bool* unfoldValue = props.unfoldValue;

		FoldLayout* fold = wm->Allocate<FoldLayout>("Fold");
		fold->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
		fold->SetAlignedPosX(0.0f);
		fold->SetAlignedSizeX(1.0f);
		fold->GetProps().tweenDuration = 0.25f;
		fold->GetProps().tweenPower	   = Theme::GetDef().baseIndentInner;
		fold->SetIsUnfolded(unfoldValue != nullptr ? *unfoldValue : false);
		fold->SetUserData(props.userData);
		fold->GetProps().onFoldChanged = [fold, unfoldValue](bool unfolded) {
			if (unfoldValue)
				*unfoldValue = unfolded;

			Icon* icon = fold->GetWidgetOfType<Icon>(fold);
			if (icon)
			{
				icon->GetProps().useAltIcon = unfolded;
				icon->CalculateIconSize();
			}
		};

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_TREEITEM);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);

		layout->GetWidgetProps().childPadding			  = Theme::GetDef().baseIndentInner;
		layout->GetWidgetProps().childMargins.left		  = props.margin;
		layout->GetWidgetProps().childMargins.right		  = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().outlineThickness		  = 0.0f;
		layout->GetWidgetProps().rounding				  = 0.0f;
		layout->GetWidgetProps().drawBackground			  = true;
		layout->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Vertical;
		layout->GetWidgetProps().colorBackground		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		// layout->GetWidgetProps().interpolateColor		  = true;
		layout->GetWidgetProps().colorInterpolateSpeed = 20.0f;
		layout->SetUserData(props.userData);
		fold->AddChild(layout);

		Icon* chevron				   = wm->Allocate<Icon>("Chevron");
		chevron->GetProps().icon	   = ICON_ARROW_RIGHT;
		chevron->GetProps().iconAlt	   = ICON_ARROW_DOWN;
		chevron->GetProps().useAltIcon = unfoldValue ? *unfoldValue : false;
		chevron->GetFlags().Set(WF_POS_ALIGN_Y);
		chevron->SetAlignedPosY(0.5f);
		chevron->SetAnchorY(Anchor::Center);
		chevron->GetProps().dynamicSizeToParent = true;
		chevron->GetProps().dynamicSizeScale	= 0.5f;
		layout->AddChild(chevron);

		if (!props.hasChildren)
			chevron->GetProps().color.start.w = chevron->GetProps().color.end.w = 0.0f;

		Icon* mainIcon			   = wm->Allocate<Icon>("MainIcon");
		mainIcon->GetProps().icon  = props.icon;
		mainIcon->GetProps().color = props.iconColor;
		mainIcon->GetFlags().Set(WF_POS_ALIGN_Y);
		mainIcon->SetAlignedPosY(0.5f);
		mainIcon->SetAnchorY(Anchor::Center);
		mainIcon->GetProps().dynamicSizeToParent = true;
		mainIcon->GetProps().dynamicSizeScale	 = 0.7f;
		layout->AddChild(mainIcon);

		Text* text = wm->Allocate<Text>("Title");
		text->UpdateTextAndCalcSize(props.title);
		text->GetProps().color = Theme::GetDef().foreground0;
		text->GetFlags().Set(WF_POS_ALIGN_Y);
		text->SetAlignedPosY(0.5f);
		text->SetAnchorY(Anchor::Center);
		layout->AddChild(text);

		Icon* vis = wm->Allocate<Icon>("Vis");
		vis->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		vis->SetAlignedPos(Vector2(1.0f, 0.5f));
		vis->SetAnchorX(Anchor::End);
		vis->SetAnchorY(Anchor::Center);
		vis->GetProps().icon				= ICON_EYE;
		vis->GetProps().dynamicSizeScale	= 0.7f;
		vis->GetProps().dynamicSizeToParent = true;

		if (props.entity)
		{
			vis->GetProps().color						  = props.entity->GetVisible() ? Theme::GetDef().foreground1 : Theme::GetDef().silent1;
			vis->GetProps().onClicked					  = [props, vis]() {
				if (props.entity->GetVisible())
				{
					props.entity->SetVisible(false);
					vis->GetProps().color = Theme::GetDef().silent1;
				}
				else
				{
					props.entity->SetVisible(true);
					vis->GetProps().color = Theme::GetDef().foreground1;
				}
			};
		}

		layout->AddChild(vis);

		fold->Initialize();
		return fold;
	}

	Widget* CommonWidgets::BuildThumbnailTooltip(void* thumbnailOwner)
	{
		Widget*		   owner = static_cast<Widget*>(thumbnailOwner);
		WidgetManager* wm	 = owner->GetWidgetManager();

		Widget* thumbnailRect = wm->Allocate<Widget>("ThumbnailRect");
		thumbnailRect->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
		thumbnailRect->SetFixedSize(Vector2(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE));
		thumbnailRect->GetWidgetProps().drawBackground	 = true;
		thumbnailRect->GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		thumbnailRect->GetWidgetProps().colorOutline	 = Theme::GetDef().outlineColorBase;
		thumbnailRect->GetWidgetProps().rounding		 = Theme::GetDef().miniRounding;
		thumbnailRect->GetWidgetProps().colorBackground	 = Theme::GetDef().background0;
		thumbnailRect->GetWidgetProps().childMargins	 = TBLR::Eq(thumbnailRect->GetWidgetProps().outlineThickness);

		Widget* thumb = wm->Allocate<Widget>("Thumb");
		thumb->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		thumb->SetAlignedPos(Vector2::Zero);
		thumb->SetAlignedSize(Vector2::One);
		thumb->GetWidgetProps().drawBackground	 = true;
		thumb->GetWidgetProps().fitTexture		 = true;
		thumb->GetWidgetProps().outlineThickness = 0.0f;
		thumb->GetWidgetProps().rounding		 = Theme::GetDef().miniRounding;
		thumb->GetWidgetProps().colorBackground	 = Color::White;
		thumb->GetWidgetProps().textureAtlas	 = owner->GetWidgetProps().textureAtlas;
		thumbnailRect->AddChild(thumb);
		return thumbnailRect;
	}

	Widget* CommonWidgets::BuildSimpleForegroundLockText(Widget* src, const String& text)
	{
		WidgetManager* wm	 = src->GetWidgetManager();
		Text*		   txt	 = wm->Allocate<Text>("Info");
		txt->GetProps().text = text;
		txt->GetProps().font = Theme::GetDef().altBigFont;
		txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		txt->SetAlignedPos(Vector2(0.5f, 0.5f));
		txt->SetAnchorX(Anchor::Center);
		txt->SetAnchorY(Anchor::Center);
		return txt;
	}

	Button* CommonWidgets::BuildIconButton(Widget* src, const String& icon)
	{
		WidgetManager* wm	  = src->GetWidgetManager();
		Button*		   button = wm->Allocate<Button>("IconButton");
		button->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
		button->SetAlignedPosY(0.0f);
		button->SetAlignedSize(Vector2::One);
		button->GetWidgetProps().drawBackground				= true;
		button->GetWidgetProps().outlineAffectedByMainColor = true;
		button->GetWidgetProps().outlineThickness			= Theme::GetDef().baseOutlineThickness;
		button->GetWidgetProps().colorBackground			= Color(0, 0, 0, 0);
		button->GetWidgetProps().colorHovered				= Theme::GetDef().background4;
		button->GetWidgetProps().colorPressed				= Theme::GetDef().background0;
		button->GetWidgetProps().colorOutline				= Theme::GetDef().silent2;
		button->GetWidgetProps().hoveredIsDifferentColor	= true;
		button->GetWidgetProps().pressedIsDifferentColor	= true;
		button->RemoveText();
		button->CreateIcon(icon);
		return button;
	}

	Widget* CommonWidgets::BuildSeperator(Widget* src)
	{
		WidgetManager* wm	= src->GetWidgetManager();
		Widget*		   line = wm->Allocate<Widget>("Line");
		line->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		line->SetAlignedPosX(0.0f);
		line->SetAlignedSizeX(1.0f);
		line->SetFixedSizeY(Theme::GetDef().baseBorderThickness);
		line->GetWidgetProps().drawBackground	= true;
		line->GetWidgetProps().outlineThickness = 0.0f;
		line->GetWidgetProps().rounding			= 0.0f;
		line->GetWidgetProps().colorBackground	= Theme::GetDef().background0;
		return line;
	}

	Widget* CommonWidgets::BuildGenericPopupWithButtons(Widget* src, const String& desc, const Vector<GenericPopupButton>& buttonDefs)
	{
		DirectionalLayout* layout = src->GetWidgetManager()->Allocate<DirectionalLayout>("Base");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_Y_TOTAL_CHILDREN | WF_SIZE_X_MAX_CHILDREN);
		layout->SetAlignedPos(Vector2(0.5f, 0.5f));
		layout->SetAlignedSize(1.0f);
		layout->SetAnchorX(Anchor::Center);
		layout->SetAnchorY(Anchor::Center);
		layout->GetProps().direction		  = DirectionOrientation::Vertical;
		layout->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent * 2;

		Text* txt = src->GetWidgetManager()->Allocate<Text>("Text");
		txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		txt->GetProps().text	  = desc;
		txt->GetProps().font	  = EDITOR_FONT_PLAY_BIG_ID;
		txt->GetProps().wrapWidth = static_cast<float>(src->GetWindow()->GetSize().x) * 0.75f;
		txt->CalculateTextSize();
		layout->AddChild(txt);

		DirectionalLayout* buttons = src->GetWidgetManager()->Allocate<DirectionalLayout>("Buttons");
		buttons->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		buttons->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		buttons->SetAlignedSizeX(1.0f);
		buttons->GetProps().mode = DirectionalLayout::Mode::EqualPositions;
		layout->AddChild(buttons);

		for (const GenericPopupButton& def : buttonDefs)
		{
			// Delegate<void()> cb = def.onPressed;

			Button* button = src->GetWidgetManager()->Allocate<Button>("Button");
			button->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
			button->SetAlignedPosY(0.0f);
			button->SetAlignedSizeY(1.0f);
			button->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4);
			button->GetText()->GetProps().text = def.title;
			button->GetProps().onClicked	   = BIND(def.onPressed);
			buttons->AddChild(button);
		}

		layout->Initialize();
		return layout;
	}

	Widget* CommonWidgets::BuildGenericPopupProgress(Widget* src, const String& desc, bool isRotatingCircle)
	{
		DirectionalLayout* layout = src->GetWidgetManager()->Allocate<DirectionalLayout>("Base");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_Y_TOTAL_CHILDREN);
		layout->SetAlignedPos(Vector2(0.5f, 0.5f));
		layout->SetAlignedSize(1.0f);
		layout->SetAnchorX(Anchor::Center);
		layout->SetAnchorY(Anchor::Center);
		layout->GetProps().direction		  = DirectionOrientation::Vertical;
		layout->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent * 2;

		Text* txt = src->GetWidgetManager()->Allocate<Text>("Text");
		txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		txt->SetAlignedPosX(0.5f);
		txt->SetAnchorX(Anchor::Center);
		txt->GetProps().text = desc;
		txt->GetProps().font = EDITOR_FONT_PLAY_BIG_ID;
		txt->CalculateTextSize();
		layout->AddChild(txt);
		layout->SetFixedSizeX(Theme::GetDef().baseItemHeight * 10);
		txt->GetProps().wrapWidth = layout->GetFixedSizeX() * 0.9f;

		ProgressCircleFill* fill = src->GetWidgetManager()->Allocate<ProgressCircleFill>("Fill");
		fill->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_COPY_X);
		fill->SetAlignedPosX(0.5f);
		fill->SetAnchorX(Anchor::Center);
		fill->SetAlignedSize(Vector2(0.5f, 1.0f));
		fill->GetProps().rotate = isRotatingCircle;
		layout->AddChild(fill);

		Text* progressText = src->GetWidgetManager()->Allocate<Text>("Progress");
		progressText->GetFlags().Set(WF_POS_ALIGN_X);
		progressText->SetAlignedPosX(0.5f);
		progressText->SetAnchorX(Anchor::Center);
		progressText->GetProps().text = Locale::GetStr(LocaleStr::Working);
		layout->AddChild(progressText);

		layout->Initialize();
		return layout;
	}

	FoldLayout* CommonWidgets::BuildFoldTitle(Widget* src, const String& title, bool* foldValue)
	{
		WidgetManager* wm = src->GetWidgetManager();

		FoldLayout* fold = wm->Allocate<FoldLayout>(title);
		fold->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
		fold->SetAlignedPosX(0.0f);
		fold->SetAlignedSizeX(1.0f);
		fold->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		fold->GetProps().tweenDuration		= 0.25f;
		fold->GetProps().tweenPower			= Theme::GetDef().baseIndentInner;
		fold->SetIsUnfolded(*foldValue);

		fold->GetProps().onFoldChanged = [fold, foldValue](bool unfolded) {
			Icon* icon			  = fold->GetWidgetOfType<Icon>(fold);
			icon->GetProps().icon = unfolded ? ICON_ARROW_DOWN : ICON_ARROW_RIGHT;
			icon->CalculateIconSize();
			*foldValue = unfolded;
		};

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>();
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(0.0f);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->GetProps().direction					  = DirectionOrientation::Horizontal;
		layout->GetWidgetProps().drawBackground			  = true;
		layout->GetWidgetProps().colorBackground.start	  = Theme::GetDef().background2;
		layout->GetWidgetProps().colorBackground.end	  = Theme::GetDef().background3;
		layout->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Horizontal;
		layout->GetWidgetProps().outlineThickness		  = Theme::GetDef().baseOutlineThickness;
		layout->GetWidgetProps().childPadding			  = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.left		  = Theme::GetDef().baseIndent * 1;
		layout->GetWidgetProps().childMargins.right		  = Theme::GetDef().baseIndent * 2;
		layout->GetWidgetProps().rounding				  = 0.0f;
		fold->AddChild(layout);

		Widget* dummyParent = wm->Allocate<Widget>();
		dummyParent->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
		dummyParent->SetAlignedPosY(0.0f);
		dummyParent->SetAlignedSizeY(1.0f);
		dummyParent->SetFixedSizeX(Theme::GetDef().baseItemHeight * 0.25f);
		layout->AddChild(dummyParent);

		Icon* chevron = wm->Allocate<Icon>("Folder");
		chevron->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		chevron->SetAlignedPos(Vector2(0.5f, 0.5f));
		chevron->SetAnchorX(Anchor::Center);
		chevron->SetAnchorY(Anchor::Center);
		chevron->GetProps().icon	  = *foldValue ? ICON_ARROW_DOWN : ICON_ARROW_RIGHT;
		chevron->GetProps().color	  = Theme::GetDef().foreground0;
		chevron->GetProps().textScale = 0.35f;
		dummyParent->AddChild(chevron);

		Text* text = wm->Allocate<Text>("Title");
		text->GetFlags().Set(WF_POS_ALIGN_Y);
		text->SetAlignedPosY(0.5f);
		text->SetAnchorY(Anchor::Center);
		text->GetProps().text = title;
		layout->AddChild(text);

		layout->GetProps().onClicked = [fold]() { fold->SetIsUnfolded(!fold->GetIsUnfolded()); };

		return fold;
	}

	Widget* CommonWidgets::BuildFieldLayout(Widget* src, uint32 dependencies, const String& title, bool isFoldLayout, bool* foldVal)
	{
		WidgetManager* wm = src->GetWidgetManager();

		FoldLayout* fold = nullptr;

		if (isFoldLayout)
		{
			fold = wm->Allocate<FoldLayout>("FieldFoldLayout");
			fold->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X);
			fold->SetAlignedPosX(0.0f);
			fold->SetAlignedSizeX(1.0f);
			fold->GetWidgetProps().childPadding		= Theme::GetDef().baseIndentInner;
			fold->GetProps().doubleClickChangesFold = false;
			fold->GetProps().lookForChevron			= false;
			fold->SetIsUnfolded(*foldVal);
			fold->GetProps().onFoldChanged = [foldVal](bool unfolded) { *foldVal = unfolded; };
			// fold->SetAlignedPosX(0.0f);
		}

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("FieldLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->SetAlignedSizeX(1.0f);
		layout->SetAlignedPosX(0.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent * 1;
		layout->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent * 2;

		if (isFoldLayout)
		{
			layout->GetFlags().Set(WF_MOUSE_PASSTHRU);
			layout->GetProps().onClicked = [fold, layout, foldVal]() {
				const Vector2 mp = layout->GetWidgetManager()->GetMousePosition();
				if (mp.x < layout->GetRect().GetCenter().x)
				{
					fold->SetIsUnfolded(!fold->GetIsUnfolded());
					*foldVal = fold->GetIsUnfolded();
				}
			};
		}

		for (int32 i = 0; i < dependencies; i++)
		{
			Widget* spacer = wm->Allocate<Widget>();
			spacer->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
			spacer->SetAlignedPosY(0.0f);
			spacer->SetAlignedSizeY(1.0f);
			spacer->SetFixedSizeX(Theme::GetDef().baseItemHeight * 0.25f);
			layout->AddChild(spacer);
		}

		Widget* dummyParent = wm->Allocate<Widget>();
		dummyParent->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
		dummyParent->SetAlignedPosY(0.0f);
		dummyParent->SetAlignedSizeY(1.0f);
		dummyParent->SetFixedSizeX(Theme::GetDef().baseItemHeight * 0.25f);
		layout->AddChild(dummyParent);

		if (isFoldLayout)
			fold->AddChild(layout);

		for (int32 i = 0; i < dependencies; i++)
		{
			Icon* icn			  = wm->Allocate<Icon>("FieldIcon");
			icn->GetProps().icon  = ICON_L;
			icn->GetProps().color = Theme::GetDef().silent2;
			icn->GetFlags().Set(WF_POS_ALIGN_Y);
			icn->SetAlignedPosY(0.5f);
			icn->SetAnchorY(Anchor::Center);

			// icn->GetFlags().Set(WF_HIDE, i != dependencies - 1);
			layout->AddChild(icn);
		}

		if (isFoldLayout)
		{
			fold->GetProps().lookForChevron = true;
			Icon* icn						= wm->Allocate<Icon>("FieldIcon");
			icn->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			icn->SetAlignedPos(Vector2(0.5f, 0.5f));
			icn->SetAnchorX(Anchor::Center);
			icn->SetAnchorY(Anchor::Center);
			icn->GetProps().icon	  = *foldVal ? ICON_ARROW_DOWN : ICON_ARROW_RIGHT;
			icn->GetProps().color	  = Theme::GetDef().foreground0;
			icn->GetProps().textScale = 0.3f;
			dummyParent->AddChild(icn);
			fold->GetProps().onFoldChanged = [icn](bool unfolded) { icn->GetProps().icon = unfolded ? ICON_ARROW_DOWN : ICON_ARROW_RIGHT; };
		}

		Text* txt			 = wm->Allocate<Text>("FieldTitle");
		txt->GetProps().text = title;
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		layout->AddChild(txt);

		DirectionalLayout* rightSide = nullptr;
		rightSide					 = wm->Allocate<DirectionalLayout>("RightSide");
		rightSide->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_ALIGN_X);
		rightSide->GetProps().direction			 = DirectionOrientation::Horizontal;
		rightSide->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		rightSide->SetAlignedPosX(1.0f);
		rightSide->SetAlignedPosY(0.0f);
		rightSide->SetAlignedSizeX(0.6f);
		rightSide->SetAlignedSizeY(1.0f);
		rightSide->SetAnchorX(Anchor::End);
		rightSide->GetWidgetProps().debugName = title;

		Widget* retVal = isFoldLayout ? static_cast<Widget*>(fold) : static_cast<Widget*>(layout);

		if (isFoldLayout)
			retVal->GetChildren().front()->AddChild(rightSide);
		else
			layout->AddChild(rightSide);

		return retVal;
	}

	namespace
	{
		uint32 CountDependencies(MetaType* type, FieldBase* field)
		{
			const Vector<DependencyPair>& depPos = field->GetPositiveDependencies();

			uint32 deps = 0;

			HashSet<FieldBase*> depSrcs;

			for (const DependencyPair& pair : depPos)
			{
				deps++;
				depSrcs.insert(type->GetField(pair.id));
			}

			for (FieldBase* f : depSrcs)
				deps += CountDependencies(type, f);

			return deps;
		}

	} // namespace

	void CommonWidgets::RefreshVector(Widget* owningFold, FieldBase* field, void* vectorPtr, MetaType* meta, FieldType subType, int32 elementIndex, bool disallowAddDelete)
	{
		FoldLayout* fold		   = static_cast<FoldLayout*>(owningFold);
		Widget*		foldFirstChild = fold->GetChildren().front();
		const int32 foldChildSz	   = static_cast<int32>(fold->GetChildren().size());
		for (int32 i = 1; i < foldChildSz; i++)
			fold->GetWidgetManager()->Deallocate(fold->GetChildren().at(i));
		fold->RemoveAllChildren();
		fold->AddChild(foldFirstChild);

		const uint32 vs = field->GetFunction<uint32(void*)>("GetVectorSize"_hs)(vectorPtr);

		Text* count			   = Widget::GetWidgetOfType<Text>(foldFirstChild->GetChildren().back());
		count->GetProps().text = "(" + TO_STRING(vs) + ")";
		count->CalculateTextSize();

		for (int32 j = 0; j < vs; j++)
		{
			void* element = field->GetFunction<void*(void*, int32)>("GetElementAddr"_hs)(vectorPtr, j);

			FieldProperties properties = {
				.type		  = subType,
				.elementIndex = j,
			};

			FillFieldProperties(properties, meta, field);

			Widget* subField = BuildField(fold, TO_STRING(j), element, properties);
			fold->AddChild(subField);

			Widget* subFieldRightSide = nullptr;

			if (subField->GetTID() == GetTypeID<FoldLayout>())
				subFieldRightSide = subField->GetChildren().front()->GetChildren().back();
			else
				subFieldRightSide = subField->GetChildren().back();

			subFieldRightSide->GetWidgetProps().childMargins = {};

			Button* duplicate = fold->GetWidgetManager()->Allocate<Button>();
			duplicate->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
			duplicate->SetAlignedPosY(0.0f);
			duplicate->SetAlignedSize(Vector2::One);
			duplicate->CreateIcon(ICON_COPY);
			duplicate->GetProps().onClicked = [j, duplicate, fold, field, vectorPtr, meta, subType, elementIndex, disallowAddDelete]() {
				duplicate->PropagateCBOnEditStarted();
				field->GetFunction<void(void*, int32)>("DuplicateElement"_hs)(vectorPtr, j);
				duplicate->PropagateCBOnEditEnded();
				RefreshVector(fold, field, vectorPtr, meta, subType, elementIndex, disallowAddDelete);
			};
			duplicate->GetFlags().Set(WF_DISABLED, disallowAddDelete);

			subFieldRightSide->AddChild(duplicate);

			Button* remove = fold->GetWidgetManager()->Allocate<Button>();
			remove->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
			remove->SetAlignedPosY(0.0f);
			remove->SetAlignedSize(Vector2::One);
			remove->CreateIcon(ICON_MINUS);
			remove->GetProps().onClicked = [j, remove, fold, field, vectorPtr, meta, subType, elementIndex, disallowAddDelete]() {
				remove->PropagateCBOnEditStarted();
				field->GetFunction<void(void*, int32)>("RemoveElement"_hs)(vectorPtr, j);
				remove->PropagateCBOnEditEnded();
				RefreshVector(fold, field, vectorPtr, meta, subType, elementIndex, disallowAddDelete);
			};
			remove->GetFlags().Set(WF_DISABLED, disallowAddDelete);
			subFieldRightSide->AddChild(remove);
		}
	}

	Widget* CommonWidgets::BuildResourceField(Widget* src, ResourceID* currentResourceID, TypeID targetType)
	{
		WidgetManager*	   wm			   = src->GetWidgetManager();
		ResourceDirectory* currentResource = Editor::Get()->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(*currentResourceID);

		Button* btn = wm->Allocate<Button>();
		btn->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		btn->SetAlignedSize(Vector2(0.0f, 1.0f));
		btn->SetAlignedPosY(0.0f);
		btn->RemoveText();

		DirectionalLayout* layout				   = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetWidgetProps().childMargins.left = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
		btn->AddChild(layout);

		Widget* thumb = wm->Allocate<Widget>("Thumb");
		thumb->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
		thumb->SetAlignedPosY(0.5f);
		thumb->SetAlignedSize(Vector2(1.0f, 0.5f));
		thumb->SetAnchorY(Anchor::Center);
		thumb->GetWidgetProps().drawBackground	 = true;
		thumb->GetWidgetProps().colorBackground	 = Color::White;
		thumb->GetWidgetProps().outlineThickness = 0.0f;
		thumb->GetWidgetProps().rounding		 = 0.0f;
		thumb->SetCustomTooltipUserData(thumb);
		thumb->GetWidgetProps().textureAtlas = currentResource == nullptr ? nullptr : Editor::Get()->GetProjectManager().GetThumbnail(currentResource);
		thumb->SetBuildCustomTooltip(BIND(&CommonWidgets::BuildThumbnailTooltip, std::placeholders::_1));
		layout->AddChild(thumb);

		Text* txt = wm->Allocate<Text>("Title");
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		txt->GetProps().text = currentResource == nullptr ? "(NoResource)" : currentResource->name;
		txt->CalculateTextSize();
		txt->GetProps().fetchCustomClipFromParent = true;
		txt->GetProps().isDynamic				  = true;
		layout->AddChild(txt);

		btn->GetProps().onClicked = [targetType, currentResourceID, btn, thumb, txt]() {
			btn->PropagateCBOnEditStarted();

			PanelGenericSelector* panel = static_cast<PanelGenericSelector*>(Editor::Get()->GetWindowPanelManager().OpenPanel(PanelType::GenericSelector, 0, nullptr));
			panel->DeallocAllChildren();
			panel->RemoveAllChildren();
			const float width = panel->GetWindow()->GetMonitorSize().x * 0.25f;
			panel->GetWindow()->SetSize({static_cast<uint32>(width), static_cast<uint32>(width * 1.15f)});

			const float posx = static_cast<float>(btn->GetWindow()->GetPosition().x) + btn->GetRect().pos.x / btn->GetWindow()->GetDPIScale();
			const float posy = static_cast<float>(btn->GetWindow()->GetPosition().y) + (btn->GetRect().pos.y + btn->GetRect().size.y) / btn->GetWindow()->GetDPIScale() + Theme::GetDef().baseIndent;

			panel->GetWindow()->SetPosition({static_cast<int32>(posx), static_cast<int32>(posy)});

			ResourceDirectoryBrowser* dirBrowser = panel->GetWidgetManager()->Allocate<ResourceDirectoryBrowser>();
			dirBrowser->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			dirBrowser->SetAlignedPos(Vector2::Zero);
			dirBrowser->SetAlignedSize(Vector2(1.0f, 0.0f));
			dirBrowser->GetProps().itemTypeIDFilter = targetType;

			ItemController* ic = dirBrowser->GetItemController();

			ic->GetProps().onItemSelected = [currentResourceID, btn, thumb, txt](void* item) {
				if (item == nullptr)
					return;
				ResourceDirectory* dir = static_cast<ResourceDirectory*>(item);
				if (dir->isFolder)
					return;

				if (*currentResourceID == dir->resourceID)
					return;
				btn->PropagateCBOnEditStarted();
				*currentResourceID = dir->resourceID;
				btn->PropagateCBOnEditEnded();
				thumb->GetWidgetProps().textureAtlas = Editor::Get()->GetProjectManager().GetThumbnail(dir);
				txt->UpdateTextAndCalcSize(dir->name);
			};

			ic->GetProps().onInteract = [ic, currentResourceID, btn, panel]() {
				Vector<ResourceDirectory*> dirs = ic->GetSelectedUserData<ResourceDirectory>();
				ResourceDirectory*		   dir	= dirs.front();
				if (dir->isFolder)
					return;

				if (*currentResourceID != dir->resourceID)
				{
					btn->PropagateCBOnEditStarted();
					*currentResourceID = dir->resourceID;
					btn->PropagateCBOnEditEnded();
				}

				Editor::Get()->GetWindowPanelManager().CloseWindow(static_cast<StringID>(panel->GetWindow()->GetSID()));
			};
			dirBrowser->Initialize();

			ResourceDirectory* dir = Editor::Get()->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(*currentResourceID);

			if (dir != nullptr)
			{
				Widget* it = ic->GetItem(dir);
				if (it)
				{
					ic->MakeVisibleRecursively(it);
					ic->SelectItem(it, true, false);
				}
			}

			panel->AddChild(dirBrowser);
		};

		return btn;
	}

	Widget* CommonWidgets::BuildEntityField(Widget* src, EntityID* currentEntityID)
	{
		WidgetManager* wm			 = src->GetWidgetManager();
		Entity*		   currentEntity = nullptr;
		EntityWorld*   world		 = nullptr;
		Panel*		   panel		 = Editor::Get()->GetWindowPanelManager().FindPanelOfType(PanelType::World, 0);
		if (panel)
		{
			world		  = static_cast<PanelWorld*>(panel)->GetWorld();
			currentEntity = world ? world->GetEntity(*currentEntityID) : nullptr;
		}

		Button* btn = wm->Allocate<Button>();
		btn->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		btn->SetAlignedSize(Vector2(0.0f, 1.0f));
		btn->SetAlignedPosY(0.0f);
		btn->RemoveText();

		DirectionalLayout* layout				   = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetWidgetProps().childMargins.left = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
		btn->AddChild(layout);

		Icon* icn = wm->Allocate<Icon>("Icon");
		icn->GetFlags().Set(WF_POS_ALIGN_Y);
		icn->SetAlignedPosY(0.5f);
		icn->SetAnchorY(Anchor::Center);
		icn->GetProps().icon				= ICON_CUBE;
		icn->GetProps().dynamicSizeScale	= 0.7f;
		icn->GetProps().dynamicSizeToParent = true;
		layout->AddChild(icn);

		Text* txt = wm->Allocate<Text>("Title");
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		txt->GetProps().text = currentEntity ? currentEntity->GetName() : "NoEntity";
		txt->CalculateTextSize();
		txt->GetProps().fetchCustomClipFromParent = true;
		txt->GetProps().isDynamic				  = true;
		layout->AddChild(txt);

		btn->GetProps().onClicked = [currentEntityID, btn, txt, currentEntity, world]() {
			btn->PropagateCBOnEditStarted();

			PanelGenericSelector* panel = static_cast<PanelGenericSelector*>(Editor::Get()->GetWindowPanelManager().OpenPanel(PanelType::GenericSelector, 0, nullptr));
			panel->DeallocAllChildren();
			panel->RemoveAllChildren();
			const float width = panel->GetWindow()->GetMonitorSize().x * 0.25f;
			panel->GetWindow()->SetSize({static_cast<uint32>(width), static_cast<uint32>(width * 1.15f)});

			const float posx = static_cast<float>(btn->GetWindow()->GetPosition().x) + btn->GetRect().pos.x / btn->GetWindow()->GetDPIScale();
			const float posy = static_cast<float>(btn->GetWindow()->GetPosition().y) + (btn->GetRect().pos.y + btn->GetRect().size.y) / btn->GetWindow()->GetDPIScale() + Theme::GetDef().baseIndent;

			panel->GetWindow()->SetPosition({static_cast<int32>(posx), static_cast<int32>(posy)});

			EntityBrowser* dirBrowser = panel->GetWidgetManager()->Allocate<EntityBrowser>();
			dirBrowser->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			dirBrowser->SetAlignedPos(Vector2::Zero);
			dirBrowser->SetAlignedSize(Vector2(1.0f, 0.0f));

			ItemController* ic = dirBrowser->GetItemController();
			dirBrowser->SetWorld(world);

			ic->GetProps().onItemSelected = [currentEntityID, btn, txt](void* item) {
				if (item == nullptr)
					return;

				Entity* entity = static_cast<Entity*>(item);
				if (*currentEntityID == entity->GetGUID())
					return;

				btn->PropagateCBOnEditStarted();
				*currentEntityID = entity->GetGUID();
				btn->PropagateCBOnEditEnded();
				txt->UpdateTextAndCalcSize(entity->GetName());
			};

			ic->GetProps().onInteract = [ic, currentEntityID, btn, panel]() {
				Vector<Entity*> dirs   = ic->GetSelectedUserData<Entity>();
				Entity*			entity = dirs.front();

				if (*currentEntityID != entity->GetGUID())
				{
					btn->PropagateCBOnEditStarted();
					*currentEntityID = entity->GetGUID();
					btn->PropagateCBOnEditEnded();
				}

				Editor::Get()->GetWindowPanelManager().CloseWindow(static_cast<StringID>(panel->GetWindow()->GetSID()));
			};
			dirBrowser->Initialize();

			Entity* cEntity = nullptr;

			if (cEntity != nullptr)
			{
				Widget* it = ic->GetItem(cEntity);
				if (it)
				{
					ic->MakeVisibleRecursively(it);
					ic->SelectItem(it, true, false);
				}
			}

			panel->AddChild(dirBrowser);
		};

		return btn;
	}

	InputField* CommonWidgets::BuildFloatField(Widget* src, void* ptr, uint8 bits, bool isInt, bool isUnsigned, bool hasLimits, float minLimit, float maxLimit, float step, bool canSelectThemeValues, uint32 decimals)
	{
		WidgetManager* wm  = src->GetWidgetManager();
		InputField*	   inp = wm->Allocate<InputField>();
		inp->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		inp->SetAlignedSize(Vector2(0.0f, 1.0f));
		inp->SetAlignedPosY(0.0f);
		inp->GetProps().isNumberField = true;
		inp->GetProps().clampNumber	  = true;

		if (decimals != 0)
			inp->GetProps().decimals = decimals;

		if (hasLimits)
		{
			inp->GetProps().disableNumberSlider = Math::Equals(step, 0.0f, 0.001f);
			inp->GetProps().valueMin			= minLimit;
			inp->GetProps().valueMax			= maxLimit;
			inp->GetProps().valueStep			= step;
		}
		else
		{
			inp->GetProps().disableNumberSlider = true;
			inp->GetProps().valueMin			= isUnsigned ? 0 : INPF_VALUE_MIN - 1.0f;
			inp->GetProps().valueMax			= INPF_VALUE_MAX + 1.0f;
			inp->GetProps().valueStep			= 0.0f;
		}

		inp->GetProps().valuePtr	  = reinterpret_cast<uint8*>(ptr);
		inp->GetProps().valueBits	  = bits;
		inp->GetProps().valueUnsigned = isUnsigned;

		if (canSelectThemeValues)
		{
			inp->GetProps().onRightClick = [wm, inp]() {
				Popup* popup				   = wm->Allocate<Popup>("Popup");
				popup->GetProps().selectedIcon = ICON_CIRCLE_FILLED;
				popup->SetPos(inp->GetPos());
				popup->AddTitleItem("Theme::");
				popup->AddToggleItem("Theme:BaseIndent", false, 0);
				popup->AddToggleItem("Theme:BaseIndentInner", false, 1);
				popup->AddToggleItem("Theme:BaseItemHeight", false, 2);
				popup->AddToggleItem("Theme:BaseRounding", false, 3);
				popup->AddToggleItem("Theme:MiniRounding", false, 4);
				popup->AddToggleItem("Theme:BaseOutlineThickness", false, 5);
				popup->AddToggleItem("Theme:BaseBorderThickness", false, 6);
				popup->GetProps().onSelectedItem = [inp](int32 idx, void* ud) {
					float val = 0.0f;

					if (idx == 0)
						val = Theme::GetDef().baseIndent;
					else if (idx == 1)
						val = Theme::GetDef().baseIndentInner;
					else if (idx == 2)
						val = Theme::GetDef().baseItemHeight;
					else if (idx == 3)
						val = Theme::GetDef().baseRounding;
					else if (idx == 4)
						val = Theme::GetDef().miniRounding;
					else if (idx == 5)
						val = Theme::GetDef().baseOutlineThickness;
					else if (idx == 6)
						val = Theme::GetDef().baseBorderThickness;

					inp->SetValue(val);
				};
				popup->Initialize();
				wm->AddToForeground(popup);
				wm->GrabControls(popup);
			};
		}

		return inp;
	}

	Widget* CommonWidgets::BuildField(Widget* src, const String& title, void* memberVariablePtr, const FieldProperties& props)
	{
		WidgetManager* wm = src->GetWidgetManager();

		FieldValue reflectionValue(memberVariablePtr);

		const bool isFold	 = props.type == FieldType::Vector || (props.type == FieldType::UserClass);
		bool*	   foldValue = nullptr;

		Widget* fieldLayout = BuildFieldLayout(src, props.dependencies + (props.elementIndex == -1 ? 0 : 1), title, isFold, props.foldPtr);

		DirectionalLayout* rightSide = nullptr;
		if (isFold)
			rightSide = Widget::GetWidgetOfType<DirectionalLayout>(fieldLayout->GetChildren().front());
		else
			rightSide = Widget::GetWidgetOfType<DirectionalLayout>(fieldLayout);

		if (props.type == FieldType::ResourceID)
		{
			ResourceID*		   rid = reflectionValue.CastPtr<ResourceID>();
			ResourceDirectory* dir = Editor::Get()->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(*rid);
			Widget*			   f   = BuildResourceField(src, rid, props.tid);
			rightSide->AddChild(f);
		}
		else if (props.type == FieldType::EntityID)
		{
			EntityID* eid = reflectionValue.CastPtr<EntityID>();
			Widget*	  f	  = BuildEntityField(src, eid);
			rightSide->AddChild(f);
		}
		else if (props.type == FieldType::UserClass)
		{
			CommonWidgets::BuildClassReflection(fieldLayout, reflectionValue.GetPtr(), ReflectionSystem::Get().Resolve(props.tid));
		}
		else if (props.type == FieldType::StringFixed)
		{
			String* str				 = reflectionValue.CastPtr<String>();
			Text*	txt				 = wm->Allocate<Text>();
			txt->GetProps().valuePtr = str;
			txt->GetFlags().Set(WF_POS_ALIGN_Y);
			txt->SetAlignedPosY(0.5f);
			txt->SetAnchorY(Anchor::Center);
			txt->GetProps().text = *str;
			txt->SetUserData(str);
			rightSide->AddChild(txt);
		}
		else if (props.type == FieldType::Vector)
		{
			void*			vectorPtr  = reflectionValue.GetPtr();
			const uint32	vectorSize = props.field->GetFunction<uint32(void*)>("GetVectorSize"_hs)(vectorPtr);
			const FieldType subType	   = props.field->GetProperty<FieldType>("SubType"_hs);

			const bool disallowAddDelete = props.field->HasProperty<uint8>("Lock0"_hs);

			rightSide->GetProps().mode = DirectionalLayout::Mode::EqualSizes;

			Text* elemCount = wm->Allocate<Text>();
			elemCount->GetFlags().Set(WF_POS_ALIGN_Y);
			elemCount->SetAlignedPosY(0.5f);
			elemCount->SetAnchorY(Anchor::Center);
			elemCount->GetProps().text = "(" + TO_STRING(vectorSize) + ")";
			rightSide->AddChild(elemCount);

			Button* newElem = wm->Allocate<Button>();
			newElem->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
			newElem->SetAlignedPosY(0.0f);
			newElem->SetAlignedSizeY(1.0f);
			newElem->CreateIcon(ICON_PLUS);
			newElem->GetProps().onClicked = [newElem, src, props, subType, fieldLayout, vectorPtr, disallowAddDelete]() {
				newElem->PropagateCBOnEditStarted();
				const uint32 vs = props.field->GetFunction<uint32(void*)>("GetVectorSize"_hs)(vectorPtr);
				props.field->GetFunction<void(void*)>("AddNewElement"_hs)(vectorPtr);
				CommonWidgets::RefreshVector(fieldLayout, props.field, vectorPtr, props.meta, subType, -1, disallowAddDelete);
				newElem->PropagateCBOnEditEnded();
			};
			newElem->GetFlags().Set(WF_DISABLED, disallowAddDelete);
			rightSide->AddChild(newElem);

			Button* clear = wm->Allocate<Button>();
			clear->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
			clear->SetAlignedPosY(0.0f);
			clear->SetAlignedSizeY(1.0f);
			clear->CreateIcon(ICON_TRASH);
			clear->GetProps().onClicked = [src, clear, props, subType, fieldLayout, vectorPtr, disallowAddDelete]() {
				clear->PropagateCBOnEditStarted();
				props.field->GetFunction<void(void*)>("ClearVector"_hs)(vectorPtr);
				CommonWidgets::RefreshVector(fieldLayout, props.field, vectorPtr, props.meta, subType, -1, disallowAddDelete);
				clear->PropagateCBOnEditEnded();
			};
			clear->GetFlags().Set(WF_DISABLED, disallowAddDelete);

			RefreshVector(fieldLayout, props.field, vectorPtr, props.meta, subType, -1, disallowAddDelete);
			rightSide->AddChild(clear);
		}
		else if (props.type == FieldType::Bitmask32)
		{
			Dropdown* dd = wm->Allocate<Dropdown>();
			dd->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			dd->SetAlignedSize(Vector2(0.0f, 1.0f));
			dd->SetAlignedPosY(0.0f);

			Bitmask32* mask				   = reflectionValue.CastPtr<Bitmask32>();
			dd->GetProps().closeOnSelect   = false;
			dd->GetText()->GetProps().text = TO_STRING(mask->GetValue());

			MetaType*			   subType = ReflectionSystem::Get().Resolve(props.tid);
			PropertyCache<String>* cache   = subType->GetPropertyCacheManager().GetPropertyCache<String>();
			Vector<String>		   values  = cache->GetSortedVector();

			dd->GetProps().onSelected = [mask, dd](int32 item, String& outNewTitle) -> bool {
				const uint32 bmVal = 1 << item;

				if (mask->IsSet(bmVal))
					mask->Remove(bmVal);
				else
					mask->Set(bmVal);

				outNewTitle = TO_STRING(mask->GetValue());

				dd->PropagateCBOnEditEnded();

				return mask->IsSet(bmVal);
			};

			dd->GetProps().onAddItems = [values, mask](Popup* popup) {
				const int32 sz = static_cast<int32>(values.size());
				for (int32 i = 0; i < sz; i++)
					popup->AddToggleItem(values[i], mask->IsSet(1 << i), i);
			};

			rightSide->AddChild(dd);
		}
		else if (props.type == FieldType::Enum)
		{
			Dropdown* dd = wm->Allocate<Dropdown>();
			dd->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			dd->SetAlignedSize(Vector2(0.0f, 1.0f));
			dd->SetAlignedPosY(0.0f);

			MetaType*			   subType = ReflectionSystem::Get().Resolve(props.tid);
			PropertyCache<String>* cache   = subType->GetPropertyCacheManager().GetPropertyCache<String>();
			Vector<String>		   values  = cache->GetSortedVector();

			int32* enumVal = reflectionValue.CastPtr<int32>();

			const Vector<uint32>& onlyShow = props.onlyShow;

			dd->GetProps().onSelected = [enumVal, values, dd](int32 item, String& outNewTitle) -> bool {
				dd->PropagateCBOnEditStarted();
				*enumVal	= item;
				outNewTitle = values[item];
				dd->PropagateCBOnEditEnded();
				return true;
			};

			dd->GetProps().onAddItems = [values, enumVal, onlyShow](Popup* popup) {
				const int32 sz = static_cast<int32>(values.size());
				for (int32 i = 0; i < sz; i++)
				{
					if (!onlyShow.empty())
					{
						auto it = linatl::find_if(onlyShow.begin(), onlyShow.end(), [i](uint32 val) -> bool { return static_cast<uint32>(i) == val; });
						if (it != onlyShow.end())
							popup->AddToggleItem(values[i], i == *enumVal, i);
					}
					else
						popup->AddToggleItem(values[i], i == *enumVal, i);
				}
			};

			dd->GetText()->GetProps().text = values[*enumVal];
			dd->GetText()->CalculateTextSize();

			rightSide->AddChild(dd);
		}
		else if (props.type == FieldType::Vector2)
		{
			Vector2* val = reflectionValue.CastPtr<Vector2>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Vector2ui)
		{
			Vector2ui* val = reflectionValue.CastPtr<Vector2ui>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Vector2i)
		{
			Vector2i* val = reflectionValue.CastPtr<Vector2i>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Vector3)
		{
			Vector3* val = reflectionValue.CastPtr<Vector3>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->z, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Vector3ui)
		{
			Vector3ui* val = reflectionValue.CastPtr<Vector3ui>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->z, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Vector3i)
		{
			Vector3i* val = reflectionValue.CastPtr<Vector3i>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->z, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Vector4)
		{
			Vector4* val = reflectionValue.CastPtr<Vector4>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->z, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->w, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Vector4ui)
		{
			Vector4ui* val = reflectionValue.CastPtr<Vector4ui>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->z, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->w, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Vector4i)
		{
			Vector4i* val = reflectionValue.CastPtr<Vector4i>();
			rightSide->AddChild(BuildFloatField(rightSide, &val->x, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->y, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->z, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &val->w, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Rect)
		{
			Rect* rect = reflectionValue.CastPtr<Rect>();
			rightSide->AddChild(BuildFloatField(rightSide, &rect->pos.x, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->pos.y, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->size.x, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->size.y, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Recti)
		{
			Recti* rect = reflectionValue.CastPtr<Recti>();
			rightSide->AddChild(BuildFloatField(rightSide, &rect->pos.x, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->pos.y, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->size.x, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->size.y, 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Rectui)
		{
			Rectui* rect = reflectionValue.CastPtr<Rectui>();
			rightSide->AddChild(BuildFloatField(rightSide, &rect->pos.x, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->pos.y, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->size.x, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &rect->size.y, 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::TBLR)
		{
			TBLR* tblr = reflectionValue.CastPtr<TBLR>();
			rightSide->AddChild(BuildFloatField(rightSide, &tblr->top, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &tblr->bottom, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &tblr->left, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
			rightSide->AddChild(BuildFloatField(rightSide, &tblr->right, 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}
		else if (props.type == FieldType::Color)
		{
			Color*		col = reflectionValue.CastPtr<Color>();
			ColorField* cf	= wm->Allocate<ColorField>();
			cf->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			cf->SetAlignedSize(Vector2(0.0f, 1.0f));
			cf->SetAlignedPosY(0.0f);
			cf->GetProps().backgroundTexture = Editor::Get()->GetApp()->GetResourceManager().GetResource<Texture>(EDITOR_TEXTURE_CHECKERED_ID);
			cf->GetProps().value			 = col;
			cf->GetProps().onClicked		 = [cf, col]() {
				PanelColorWheel* panel		   = Editor::Get()->GetWindowPanelManager().OpenColorWheelPanel(cf);
				panel->GetCallbacks().onEdited = [col, panel, cf]() {
					*col = panel->GetWheel()->GetEditedColor().SRGB2Linear();
					cf->PropagateCBOnEdited();
				};
				panel->GetCallbacks().onEditEnded	= [cf]() { cf->PropagateCBOnEditEnded(); };
				panel->GetCallbacks().onEditStarted = [cf]() { cf->PropagateCBOnEditStarted(); };
				panel->GetWheel()->SetTargetColor(*col);
			};
			rightSide->AddChild(cf);
		}
		else if (props.type == FieldType::ColorGrad)
		{
			ColorGrad* col = reflectionValue.CastPtr<ColorGrad>();
			rightSide->AddChild(BuildColorGradSlider(src, col, props.meta, props.field));
		}
		else if (props.type == FieldType::String)
		{
			String*		strVal = reflectionValue.CastPtr<String>();
			InputField* inp	   = wm->Allocate<InputField>();
			inp->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			inp->SetAlignedSize(Vector2(0.0f, 1.0f));
			inp->SetAlignedPosY(0.0f);
			inp->GetText()->GetProps().text = *strVal;
			inp->GetProps().valueStr		= strVal;
			rightSide->AddChild(inp);
		}
		else if (props.type == FieldType::Boolean)
		{
			rightSide->GetProps().mode = DirectionalLayout::Mode::Default;

			bool*	  bval = reflectionValue.CastPtr<bool>();
			Checkbox* cb   = wm->Allocate<Checkbox>();
			cb->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
			cb->SetAlignedSize(Vector2::One);
			cb->SetAlignedPosY(0.0f);
			cb->GetProps().value		   = bval;
			cb->GetIcon()->GetProps().icon = ICON_CHECK;
			cb->GetIcon()->CalculateIconSize();
			rightSide->AddChild(cb);
		}
		else if (props.type == FieldType::UInt32)
		{
			rightSide->AddChild(BuildFloatField(rightSide, reflectionValue.GetPtr(), 32, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat, false, 0));
		}
		else if (props.type == FieldType::Int32)
		{
			rightSide->AddChild(BuildFloatField(rightSide, reflectionValue.GetPtr(), 32, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat, false, 0));
		}
		else if (props.type == FieldType::UInt16)
		{
			rightSide->AddChild(BuildFloatField(rightSide, reflectionValue.GetPtr(), 16, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat, false, 0));
		}
		else if (props.type == FieldType::Int16)
		{
			rightSide->AddChild(BuildFloatField(rightSide, reflectionValue.GetPtr(), 16, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat, false, 0));
		}
		else if (props.type == FieldType::UInt8)
		{
			rightSide->AddChild(BuildFloatField(rightSide, reflectionValue.GetPtr(), 8, true, true, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat, false, 0));
		}
		else if (props.type == FieldType::Int8)
		{
			rightSide->AddChild(BuildFloatField(rightSide, reflectionValue.GetPtr(), 8, true, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat, false, 0));
		}
		else if (props.type == FieldType::Float)
		{
			rightSide->AddChild(BuildFloatField(rightSide, reflectionValue.GetPtr(), 32, false, false, props.hasLimits, props.minFloat, props.maxFloat, props.stepFloat));
		}

		fieldLayout->GetWidgetProps().tooltip = props.tooltip;
		fieldLayout->Initialize();
		fieldLayout->SetUserData(props.field);
		return fieldLayout;
	}

	Widget* CommonWidgets::BuildColorGradSlider(Widget* src, ColorGrad* color, MetaType* metaType, FieldBase* field)
	{
		WidgetManager*	   wm	  = src->GetWidgetManager();
		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>();
		layout->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedSize(Vector2(0.0f, 1.0f));
		layout->SetAlignedPosY(0.0f);
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndentInner / 2;

		Button* startButton = wm->Allocate<Button>();
		startButton->GetFlags().Set(WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		startButton->SetAlignedPosY(0.0f);
		startButton->SetAlignedSize(Vector2::One);
		startButton->CreateIcon(ICON_PALETTE);
		startButton->GetWidgetProps().tooltip = Locale::GetStr(LocaleStr::StartColor);
		startButton->GetProps().onClicked	  = [startButton, color]() {
			PanelColorWheel* pcw = Editor::Get()->GetWindowPanelManager().OpenColorWheelPanel(startButton);

			pcw->GetWheel()->SetTargetColor(color->start);
			pcw->GetCallbacks().onEdited	= [color, pcw, startButton]() { const Color col = pcw->GetWheel()->GetEditedColor().SRGB2Linear(); };
			pcw->GetCallbacks().onEditEnded = [startButton]() { startButton->PropagateCBOnEditEnded(); };
		};
		layout->AddChild(startButton);

		Button* middleButton = wm->Allocate<Button>();
		middleButton->GetFlags().Set(WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		middleButton->SetAlignedPosY(0.0f);
		middleButton->SetAlignedSize(Vector2::One);
		middleButton->CreateIcon(ICON_PALETTE);
		middleButton->GetWidgetProps().tooltip = Locale::GetStr(LocaleStr::Both);
		middleButton->GetProps().onClicked	   = [middleButton, color]() {
			PanelColorWheel* pcw = Editor::Get()->GetWindowPanelManager().OpenColorWheelPanel(middleButton);
			pcw->GetWheel()->SetTargetColor(color->start);
			pcw->GetCallbacks().onEdited = [color, pcw, middleButton]() {
				const Color col = pcw->GetWheel()->GetEditedColor().SRGB2Linear();
				color->start	= col;
				color->end		= col;
			};

			pcw->GetCallbacks().onEditEnded = [middleButton]() { middleButton->PropagateCBOnEditEnded(); };
		};
		layout->AddChild(middleButton);

		Button* endButton = wm->Allocate<Button>();
		endButton->GetFlags().Set(WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		endButton->SetAlignedPosY(0.0f);
		endButton->SetAlignedSize(Vector2::One);
		endButton->CreateIcon(ICON_PALETTE);
		endButton->GetWidgetProps().tooltip = Locale::GetStr(LocaleStr::EndColor);
		endButton->GetProps().onClicked		= [endButton, color]() {
			PanelColorWheel* pcw = Editor::Get()->GetWindowPanelManager().OpenColorWheelPanel(endButton);
			pcw->GetWheel()->SetTargetColor(color->start);
			pcw->GetCallbacks().onEdited = [color, pcw, endButton]() {
				const Color col = pcw->GetWheel()->GetEditedColor().SRGB2Linear();
				color->end		= col;
			};
			pcw->GetCallbacks().onEditEnded = [endButton]() { endButton->PropagateCBOnEditEnded(); };
		};
		layout->AddChild(endButton);

		ColorField* cf = wm->Allocate<ColorField>();
		cf->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		cf->SetAlignedSize(Vector2(0.0f, 1.0f));
		cf->SetAlignedPosY(0.0f);
		cf->GetProps().gradValue		 = color;
		cf->GetProps().disableInput		 = true;
		cf->GetProps().backgroundTexture = Editor::Get()->GetApp()->GetResourceManager().GetResource<Texture>(EDITOR_TEXTURE_CHECKERED_ID);
		layout->AddChild(cf);

		layout->Initialize();
		return layout;
	}

	void CommonWidgets::BuildClassReflection(Widget* owner, void* obj, MetaType* meta)
	{
		WidgetManager* wm = owner->GetWidgetManager();

		Vector<FieldBase*> fields	  = meta->GetFieldsOrdered();
		Widget*			   lastParent = owner;

		for (FieldBase* field : fields)
		{
			void*		 memberVariable = field->Value(obj).GetPtr();
			const String title			= field->GetProperty<String>("Title"_hs);

			FieldProperties props = {
				.type = field->GetProperty<FieldType>("Type"_hs),
			};

			FillFieldProperties(props, meta, field);

			Widget* fieldWidget = BuildField(owner, title, memberVariable, props);
			lastParent->AddChild(fieldWidget);
		}

		for (FieldBase* field : fields)
		{
			const Vector<DependencyPair>& posDepends = field->GetPositiveDependencies();
			if (posDepends.size() == 0)
				continue;

			Widget* current = owner->FindChildWithUserdata(field);
			if (current == nullptr)
				continue;

			for (const DependencyPair& depPair : posDepends)
			{
				const StringID		sid	 = depPair.id;
				const StringID		op	 = depPair.operation;
				const Vector<uint8> vals = depPair.deps;

				FieldBase* depSrc = meta->GetField(sid);

				void* valPtr = depSrc->Value(obj).GetPtr();
				current->AddPreTickHook([valPtr, vals, current, op]() {
					const uint8 depVal = *static_cast<uint8*>(valPtr);

					bool passes = false;

					auto	   it	 = linatl::find_if(vals.begin(), vals.end(), [depVal](uint8 v) -> bool { return v == depVal; });
					const bool found = it != vals.end();

					if (op == "eq"_hs)
						passes = found;
					else if (op == "neq"_hs)
						passes = !found;

					if (passes && current->GetFlags().IsSet(WF_HIDE))
						current->GetFlags().Remove(WF_HIDE);
					else if (!passes && !current->GetFlags().IsSet(WF_HIDE))
						current->GetFlags().Set(WF_HIDE);
				});
			}
		}
	}

	Widget* CommonWidgets::ThrowResourceSelector(Widget* src, ResourceID currentResourceID, TypeID resourceType)
	{
		WidgetManager*	   wm		 = src->GetWidgetManager();
		DirectionalLayout* layout	 = wm->Allocate<DirectionalLayout>();
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_FOREGROUND_BLOCKER);
		layout->SetAlignedPos(Vector2(0.5f, 0.5f));
		layout->SetAlignedSize(Vector2(0.4f, 0.6f));
		layout->SetAnchorX(Anchor::Center);
		layout->SetAnchorY(Anchor::Center);
		layout->GetWidgetProps().borderThickness  = TBLR::Eq(2);
		layout->GetWidgetProps().colorBorders	  = Theme::GetDef().black;
		layout->GetWidgetProps().drawBackground	  = true;
		layout->GetWidgetProps().outlineThickness = 0.0f;
		layout->GetWidgetProps().rounding		  = 0.0f;
		layout->GetWidgetProps().colorBackground  = Theme::GetDef().background1;

		WindowBar* wb = wm->Allocate<WindowBar>();
		wb->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		wb->SetAlignedPosX(0.0f);
		wb->SetAlignedSizeX(1.0f);
		wb->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->AddChild(wb);

		Text* txt			 = wm->Allocate<Text>();
		txt->GetProps().text = Locale::GetStr(LocaleStr::SelectWidget);
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		wb->AddChild(txt);

		ResourceDirectoryBrowser* bw = wm->Allocate<ResourceDirectoryBrowser>();
		bw->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bw->SetAlignedPosX(0.0f);
		bw->SetAlignedSize(Vector2(1.0f, 0.0f));
		bw->GetProps().itemTypeIDFilter				   = resourceType;
		bw->GetItemController()->GetProps().onInteract = [bw, layout, resourceType, wm]() {
			ResourceDirectory* selection = bw->GetItemController()->GetSelectedUserData<ResourceDirectory>().front();
			if (!selection->isFolder && selection->resourceTID == resourceType)
			{
				wm->AddToKillList(layout);
				// onSelected(selection);
			}
		};
		layout->AddChild(bw);

		DirectionalLayout* horizontal = wm->Allocate<DirectionalLayout>();
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		horizontal->SetAlignedPosX(0.0f);
		horizontal->SetAlignedSizeX(1.0f);
		horizontal->SetFixedSizeY(Theme::GetDef().baseItemHeight * 1.5f);
		horizontal->GetWidgetProps().drawBackground		 = true;
		horizontal->GetWidgetProps().colorBackground	 = Theme::GetDef().background1;
		horizontal->GetWidgetProps().borderThickness.top = Theme::GetDef().baseOutlineThickness;
		horizontal->GetProps().mode						 = DirectionalLayout::Mode::EqualPositions;
		horizontal->GetWidgetProps().childPadding		 = Theme::GetDef().baseIndent;
		horizontal->GetWidgetProps().childMargins		 = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		layout->AddChild(horizontal);

		Button* select					   = wm->Allocate<Button>();
		select->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Select);
		select->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		select->SetAlignedSizeX(0.3f);
		select->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		select->SetAlignedPosY(0.5f);
		select->SetAnchorY(Anchor::Center);
		select->SetTickHook([bw, select, resourceType](float delta) {
			Vector<ResourceDirectory*> selection = bw->GetItemController()->GetSelectedUserData<ResourceDirectory>();
			if (selection.size() == 1 && selection.front()->resourceTID == resourceType)
				select->GetFlags().Remove(WF_DISABLED);
			else
				select->GetFlags().Set(WF_DISABLED);
		});

		select->GetProps().onClicked = [layout, bw, wm]() {
			wm->AddToKillList(layout);
			ResourceDirectory* selection = bw->GetItemController()->GetSelectedUserData<ResourceDirectory>().front();
			// onSelected(selection);
		};

		horizontal->AddChild(select);

		Button* cancel					   = wm->Allocate<Button>();
		cancel->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Cancel);
		cancel->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_Y_TOTAL_CHILDREN | WF_SIZE_ALIGN_X);
		cancel->SetAlignedSizeX(0.3f);
		cancel->GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
		cancel->SetAlignedPosY(0.5f);
		cancel->SetAnchorY(Anchor::Center);

		cancel->GetProps().onClicked = [layout, wm]() { wm->AddToKillList(layout); };
		horizontal->AddChild(cancel);

		layout->Initialize();
		wm->AddToForeground(layout);
		wm->GrabControls(layout);

		if (currentResourceID != 0)
		{
			ResourceDirectory* currentDir = Editor::Get()->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(currentResourceID);
			Widget*			   it		  = bw->GetItemController()->GetItem(currentDir);
			if (it)
			{
				bw->GetItemController()->MakeVisibleRecursively(it);
				bw->GetItemController()->SelectItem(it, true, false);
			}
		}

		return layout;
	}

	void CommonWidgets::FillFieldProperties(FieldProperties& out, MetaType* meta, FieldBase* field)
	{
		out.dependencies = CountDependencies(meta, field);
		out.foldPtr		 = field->GetFoldValuePtr();
		out.field		 = field;
		out.meta		 = meta;

		if (field->HasProperty<Vector<uint32>>("OnlyShow"_hs))
			out.onlyShow = field->GetProperty<Vector<uint32>>("OnlyShow"_hs);

		if (field->HasProperty<float>("Min"_hs))
		{
			out.minFloat  = field->GetProperty<float>("Min"_hs);
			out.maxFloat  = field->GetProperty<float>("Max"_hs);
			out.stepFloat = field->GetProperty<float>("Step"_hs);
			out.hasLimits = true;
		}

		if (field->HasProperty<String>("Tooltip"_hs))
			out.tooltip = field->GetProperty<String>("Tooltip"_hs);
		if (field->HasProperty<TypeID>("SubType"_hs))
			out.tid = field->GetProperty<TypeID>("SubType"_hs);
		else if (field->HasProperty<TypeID>("SubTypeTID"_hs))
			out.tid = field->GetProperty<TypeID>("SubTypeTID"_hs);
	}
} // namespace Lina::Editor
