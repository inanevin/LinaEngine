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
#include "Editor/Widgets/FX/LinaLoading.hpp"
#include "Editor/Widgets/Layout/ItemController.hpp"
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/System.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"

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

	Widget* CommonWidgets::BuildPayloadForDirectoryItem(Widget* src, DirectoryItem* item)
	{
		WidgetManager*	   wm				  = src->GetWidgetManager();
		DirectionalLayout* layout			  = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_SIZE_X_TOTAL_CHILDREN | WF_USE_FIXED_SIZE_Y);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);

		if (item->isDirectory)
		{
			Icon* icon			  = wm->Allocate<Icon>("IconBG");
			icon->GetProps().icon = ICON_FOLDER;
			icon->GetFlags().Set(WF_POS_ALIGN_Y);
			icon->SetAlignedPosY(0.5f);
			icon->SetAnchorY(Anchor::Center);
			layout->AddChild(icon);
		}
		else
		{
			if (item->textureAtlas == nullptr)
			{
				LinaLoading* loading = wm->Allocate<LinaLoading>();
				loading->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
				loading->SetAlignedSizeY(1.0f);
				loading->SetAlignedPosY(0.5f);
				loading->SetAnchorX(Anchor::Center);
				loading->SetAnchorY(Anchor::Center);
				loading->GetWidgetProps().outlineThickness = 0.0f;
				loading->GetWidgetProps().rounding		   = 0.0f;
				layout->AddChild(loading);
			}
			else
			{
				Widget* bg = wm->Allocate<Widget>();
				bg->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
				bg->SetAlignedSizeY(1.0f);
				bg->GetWidgetProps().drawBackground	 = true;
				bg->GetWidgetProps().colorBackground = Color::White;
				bg->GetWidgetProps().fitTexture		 = true;
				bg->GetWidgetProps().textureAtlas	 = item->textureAtlas;
				layout->AddChild(bg);
			}
		}

		Text* txt			 = wm->Allocate<Text>("Text");
		txt->GetProps().text = item->name;
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		layout->AddChild(txt);
		layout->Initialize();
		return layout;
	}

	Widget* CommonWidgets::BuildDefaultFoldItem(Widget* src, void* userdata, float margin, const String& icon, const Color& iconColor, const String& title, bool hasChildren, bool* unfoldVal, bool isRoot, bool boldText)
	{
		WidgetManager* wm = src->GetWidgetManager();

		FoldLayout* fold = wm->Allocate<FoldLayout>("Fold");

		if (isRoot)
		{
			fold->GetFlags().Set(WF_POS_ALIGN_X);
			fold->SetAlignedPosX(0.0f);
		}

		fold->GetFlags().Set(WF_SIZE_ALIGN_X);
		fold->SetAlignedSizeX(1.0f);
		fold->GetProps().useTween	   = true;
		fold->GetProps().tweenDuration = 0.25f;
		fold->GetProps().tweenPower	   = Theme::GetDef().baseIndentInner;
		fold->SetIsUnfolded(unfoldVal != nullptr ? *unfoldVal : false);
		fold->SetUserData(userdata);

		fold->GetProps().onFoldChanged = [fold, unfoldVal](bool unfolded) {
			Icon* icon			  = fold->GetWidgetOfType<Icon>(fold);
			icon->GetProps().icon = unfolded ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT;
			icon->CalculateIconSize();

			if (unfoldVal != nullptr)
				*unfoldVal = unfolded;
		};

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->GetWidgetProps().childPadding			  = Theme::GetDef().baseIndentInner;
		layout->GetWidgetProps().childMargins.left		  = margin;
		layout->GetWidgetProps().drawBackground			  = true;
		layout->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Vertical;
		layout->GetWidgetProps().colorBackground		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		layout->GetWidgetProps().outlineThickness		  = 0.0f;
		layout->GetWidgetProps().rounding				  = 0.0f;
		layout->GetWidgetProps().interpolateColor		  = true;
		layout->GetWidgetProps().colorInterpolateSpeed	  = 20.0f;
		layout->SetUserData(userdata);
		fold->AddChild(layout);

		Icon* chevron			 = wm->Allocate<Icon>("Folder");
		chevron->GetProps().icon = fold->GetIsUnfolded() ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT;
		chevron->GetFlags().Set(WF_POS_ALIGN_Y);
		chevron->SetAlignedPosY(0.5f);
		chevron->SetAnchorY(Anchor::Center);
		chevron->GetProps().dynamicSizeToParent = true;
		chevron->GetProps().dynamicSizeScale	= 0.55f;
		layout->AddChild(chevron);
		chevron->SetVisible(hasChildren);

		if (!icon.empty())
		{
			Icon* ic = wm->Allocate<Icon>("Folder");
			ic->GetFlags().Set(WF_POS_ALIGN_Y);
			ic->SetAlignedPosY(0.5f);
			ic->GetProps().icon				   = icon;
			ic->GetProps().color			   = iconColor;
			ic->GetProps().dynamicSizeToParent = true;
			ic->GetProps().dynamicSizeScale	   = 0.65f;
			ic->SetAnchorY(Anchor::Center);
			layout->AddChild(ic);
		}

		Text* txt			 = wm->Allocate<Text>("Title");
		txt->GetProps().text = title;
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		txt->SetUserData(userdata);

		if (boldText)
			txt->GetProps().font = DEFAULT_FONT_BOLD_SID;
		layout->AddChild(txt);
		fold->Initialize();
		return fold;
	}

	Widget* CommonWidgets::BuildTexturedListItem(Widget* src, void* userData, float margin, TextureAtlasImage* img, const String& title, bool foldNudge)
	{
		WidgetManager*	   wm	  = src->GetWidgetManager();
		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->GetWidgetProps().childPadding			  = Theme::GetDef().baseIndentInner;
		layout->GetWidgetProps().childMargins.left		  = margin;
		layout->GetWidgetProps().childMargins.right		  = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().drawBackground			  = true;
		layout->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Vertical;
		layout->GetWidgetProps().colorBackground		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		layout->GetWidgetProps().outlineThickness		  = 0.0f;
		layout->GetWidgetProps().rounding				  = 0.0f;
		layout->GetWidgetProps().interpolateColor		  = true;
		layout->GetWidgetProps().colorInterpolateSpeed	  = 20.0f;
		layout->SetUserData(userData);

		if (foldNudge)
		{
			Icon* chevron			 = wm->Allocate<Icon>("Folder");
			chevron->GetProps().icon = ICON_CHEVRON_RIGHT;
			chevron->GetFlags().Set(WF_POS_ALIGN_Y);
			chevron->SetAlignedPosY(0.5f);
			chevron->SetAnchorY(Anchor::Center);
			chevron->GetProps().dynamicSizeToParent = true;
			chevron->GetProps().dynamicSizeScale	= 0.55f;
			layout->AddChild(chevron);
			chevron->SetVisible(false);
		}

		Widget* bg = wm->Allocate<Widget>("BG");
		bg->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
		bg->SetAlignedPosY(0.5f);
		bg->SetAnchorY(Anchor::Center);
		bg->SetAlignedSizeY(0.75f);
		bg->GetWidgetProps().drawBackground	  = true;
		bg->GetWidgetProps().textureAtlas	  = img;
		bg->GetWidgetProps().outlineThickness = 0.0f;
		bg->GetWidgetProps().rounding		  = 2.0f;
		bg->GetWidgetProps().colorBackground  = Color::White;
		layout->AddChild(bg);

		Text* titleText = wm->Allocate<Text>("Title");
		titleText->GetFlags().Set(WF_POS_ALIGN_Y);
		titleText->SetAlignedPosY(0.5f);
		titleText->SetAnchorY(Anchor::Center);
		titleText->GetProps().text = title;
		titleText->SetUserData(userData);
		layout->AddChild(titleText);
		layout->Initialize();
		return layout;
	}

	Widget* CommonWidgets::BuildDefaultListItem(Widget* src, void* userData, float margin, const String& icn, const Color& iconColor, const String& txt, bool foldNudge)
	{
		WidgetManager* wm = src->GetWidgetManager();

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->GetWidgetProps().childPadding			  = Theme::GetDef().baseIndentInner;
		layout->GetWidgetProps().childMargins.left		  = margin;
		layout->GetWidgetProps().childMargins.right		  = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().drawBackground			  = true;
		layout->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Vertical;
		layout->GetWidgetProps().colorBackground		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		layout->GetWidgetProps().outlineThickness		  = 0.0f;
		layout->GetWidgetProps().rounding				  = 0.0f;
		layout->GetWidgetProps().interpolateColor		  = true;
		layout->GetWidgetProps().colorInterpolateSpeed	  = 20.0f;
		layout->SetUserData(userData);

		if (foldNudge)
		{
			Icon* chevron			 = wm->Allocate<Icon>("Folder");
			chevron->GetProps().icon = ICON_CHEVRON_RIGHT;
			chevron->GetFlags().Set(WF_POS_ALIGN_Y);
			chevron->SetAlignedPosY(0.5f);
			chevron->SetAnchorY(Anchor::Center);
			chevron->GetProps().dynamicSizeToParent = true;
			chevron->GetProps().dynamicSizeScale	= 0.55f;
			layout->AddChild(chevron);
			chevron->SetVisible(false);
		}

		if (!icn.empty())
		{
			Widget* bg = wm->Allocate<Widget>("BG");
			bg->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
			bg->SetAlignedPosY(0.5f);
			bg->SetAnchorY(Anchor::Center);
			bg->SetAlignedSizeY(1.0f);
			layout->AddChild(bg);

			Icon* icon			   = wm->Allocate<Icon>("Folder");
			icon->GetProps().icon  = icn;
			icon->GetProps().color = iconColor;
			icon->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
			icon->SetAlignedPos(Vector2(0.5f));
			icon->GetProps().dynamicSizeToParent = true;
			icon->GetProps().dynamicSizeScale	 = 0.8f;
			icon->SetAnchorX(Anchor::Center);
			icon->SetAnchorY(Anchor::Center);
			bg->AddChild(icon);
		}

		Text* title = wm->Allocate<Text>("Title");
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetAnchorY(Anchor::Center);
		title->GetProps().text = txt;
		title->SetUserData(userData);
		layout->AddChild(title);
		layout->Initialize();
		return layout;
	}

	Widget* CommonWidgets::BuildDirectoryItemFolderView(Widget* src, DirectoryItem* item, float margin)
	{
		WidgetManager* wm = src->GetWidgetManager();

		FoldLayout* fold = wm->Allocate<FoldLayout>("Fold");

		if (item->parent == nullptr)
		{
			fold->GetFlags().Set(WF_POS_ALIGN_X);
			fold->SetAlignedPosX(0.0f);
		}

		fold->GetFlags().Set(WF_SIZE_ALIGN_X);
		fold->SetAlignedSizeX(1.0f);
		fold->GetProps().useTween	   = true;
		fold->GetProps().tweenDuration = 0.25f;
		fold->GetProps().tweenPower	   = Theme::GetDef().baseIndentInner;
		fold->SetIsUnfolded(item->isUnfolded);
		fold->SetUserData(item);
		fold->GetWidgetProps().debugName = item->name;

		fold->GetProps().onFoldChanged = [fold, item](bool unfolded) {
			Icon* icon			  = fold->GetWidgetOfType<Icon>(fold);
			icon->GetProps().icon = unfolded ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT;
			icon->CalculateIconSize();
			item->isUnfolded = unfolded;
		};

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->GetWidgetProps().childPadding			  = Theme::GetDef().baseIndentInner;
		layout->GetWidgetProps().childMargins.left		  = margin;
		layout->GetWidgetProps().drawBackground			  = true;
		layout->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Vertical;
		layout->GetWidgetProps().colorBackground		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		layout->GetWidgetProps().outlineThickness		  = 0.0f;
		layout->GetWidgetProps().rounding				  = 0.0f;
		layout->GetWidgetProps().interpolateColor		  = true;
		layout->GetWidgetProps().colorInterpolateSpeed	  = 20.0f;
		layout->SetUserData(item);
		fold->AddChild(layout);

		Icon* chevron			 = wm->Allocate<Icon>("Folder");
		chevron->GetProps().icon = fold->GetIsUnfolded() ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT;
		chevron->GetFlags().Set(WF_POS_ALIGN_Y);
		chevron->SetAlignedPosY(0.5f);
		chevron->GetProps().textScale = 0.4f;
		chevron->SetAnchorY(Anchor::Center);

		layout->AddChild(chevron);

		auto it = linatl::find_if(item->children.begin(), item->children.end(), [](DirectoryItem* c) -> bool { return c->isDirectory; });
		if (it == item->children.end())
			chevron->SetVisible(false);

		Icon* icon			  = wm->Allocate<Icon>("Folder");
		icon->GetProps().icon = ICON_FOLDER;
		icon->GetFlags().Set(WF_POS_ALIGN_Y);
		icon->SetAlignedPosY(0.5f);
		icon->SetAnchorY(Anchor::Center);
		layout->AddChild(icon);

		Text* title = wm->Allocate<Text>("Title");
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetAnchorY(Anchor::Center);
		title->GetProps().text = item->name;
		title->SetUserData(item);
		title->SetUserData(item);
		layout->AddChild(title);
		fold->Initialize();
		return fold;
	}

	Widget* CommonWidgets::BuildDirectoryItemListView(Widget* src, DirectoryItem* item)
	{
		WidgetManager* wm = src->GetWidgetManager();

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPosX(0);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->GetWidgetProps().childPadding			  = Theme::GetDef().baseIndentInner;
		layout->GetWidgetProps().childMargins.left		  = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins.right		  = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().drawBackground			  = true;
		layout->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Vertical;
		layout->GetWidgetProps().colorBackground		  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		layout->GetWidgetProps().outlineThickness		  = 0.0f;
		layout->GetWidgetProps().rounding				  = 0.0f;
		layout->GetWidgetProps().interpolateColor		  = true;
		layout->GetWidgetProps().colorInterpolateSpeed	  = 20.0f;
		layout->SetUserData(item);

		Widget* bg = wm->Allocate<Widget>("BG");
		bg->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
		bg->SetAlignedPosY(0.5f);
		bg->SetAnchorY(Anchor::Center);
		bg->SetAlignedSizeY(1.0f);
		layout->AddChild(bg);

		if (item->isDirectory)
		{

			Icon* icon			  = wm->Allocate<Icon>("Folder");
			icon->GetProps().icon = ICON_FOLDER;
			icon->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
			icon->SetAlignedPos(Vector2(0.5f));
			icon->GetProps().dynamicSizeToParent = true;
			icon->GetProps().dynamicSizeScale	 = 0.8f;
			icon->SetAnchorX(Anchor::Center);
			icon->SetAnchorY(Anchor::Center);
			bg->AddChild(icon);
		}
		else
		{
			if (item->textureAtlas == nullptr)
			{
				LinaLoading* loading = wm->Allocate<LinaLoading>();
				loading->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
				loading->SetAlignedPos(Vector2(0.5f, 0.5f));
				loading->SetAlignedSize(Vector2(0.25f, 0.25f));
				loading->SetAnchorX(Anchor::Center);
				loading->SetAnchorY(Anchor::Center);
				loading->GetWidgetProps().outlineThickness = 0.0f;
				loading->GetWidgetProps().rounding		   = 0.0f;
				bg->AddChild(loading);
			}
			else
			{
				bg->GetWidgetProps().drawBackground	  = true;
				bg->GetWidgetProps().colorBackground  = Color::White;
				bg->GetWidgetProps().fitTexture		  = true;
				bg->GetWidgetProps().textureAtlas	  = item->textureAtlas;
				bg->GetWidgetProps().outlineThickness = 0.0f;
				bg->GetWidgetProps().rounding		  = 0.0f;
			}
		}

		Text* title = wm->Allocate<Text>("Title");
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetAnchorY(Anchor::Center);
		title->GetProps().text = item->name;
		title->SetUserData(item);
		title->SetUserData(item);
		layout->AddChild(title);
		layout->Initialize();
		return layout;
	}

	Widget* CommonWidgets::BuildDirectoryItemGridView(Widget* src, DirectoryItem* item, const Vector2& itemSize)
	{
		WidgetManager* wm = src->GetWidgetManager();

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
		layout->SetFixedSize(itemSize);
		layout->SetUserData(item);
		layout->GetProps().direction = DirectionOrientation::Vertical;

		Widget* imageBG = wm->Allocate<Widget>("ImageBG");
		imageBG->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_COPY_X);
		imageBG->SetAlignedPosX(0.0f);
		imageBG->SetAlignedSizeX(1.0f);
		imageBG->GetWidgetProps().onlyRound.push_back(0);
		imageBG->GetWidgetProps().onlyRound.push_back(1);
		imageBG->GetWidgetProps().rounding		   = Theme::GetDef().baseRounding / 2;
		imageBG->GetWidgetProps().outlineThickness = 0.0f;
		// imageBG->GetWidgetProps().colorOutline	   = Theme::GetDef().outlineColorBase;

		// imageBG->GetWidgetProps().outlineThickness = 0.0f;
		layout->AddChild(imageBG);

		if (item->isDirectory)
		{
			Icon* icon			  = wm->Allocate<Icon>("Folder");
			icon->GetProps().icon = ICON_FOLDER;
			icon->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
			icon->SetAlignedPos(Vector2(0.5f));
			icon->GetProps().dynamicSizeToParent = true;
			icon->GetProps().dynamicSizeScale	 = 0.8f;
			icon->SetAnchorX(Anchor::Center);
			icon->SetAnchorY(Anchor::Center);
			imageBG->AddChild(icon);
		}
		else
		{
			if (item->textureAtlas == nullptr)
			{
				LinaLoading* loading = wm->Allocate<LinaLoading>();
				loading->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
				loading->SetAlignedPos(Vector2(0.5f, 0.5f));
				loading->SetAlignedSize(Vector2(0.25f, 0.25f));
				loading->SetAnchorX(Anchor::Center);
				loading->SetAnchorY(Anchor::Center);
				loading->GetWidgetProps().outlineThickness = 0.0f;
				loading->GetWidgetProps().rounding		   = 0.0f;
				imageBG->AddChild(loading);
			}
			else
			{
				imageBG->GetWidgetProps().drawBackground  = true;
				imageBG->GetWidgetProps().colorBackground = Color::White;
				imageBG->GetWidgetProps().fitTexture	  = true;
				imageBG->GetWidgetProps().textureAtlas	  = item->textureAtlas;
			}
		}

		Widget* textBG = wm->Allocate<Widget>("TextBG");
		textBG->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		textBG->SetAlignedPosX(0.0f);
		textBG->SetAlignedSize(Vector2(1.0f, 0.0f));
		textBG->GetWidgetProps().drawBackground	 = true;
		textBG->GetWidgetProps().colorBackground = Theme::GetDef().background1;
		textBG->SetUserData(item);
		textBG->GetWidgetProps().dropshadow.enabled		  = true;
		textBG->GetWidgetProps().dropshadow.direction	  = Direction::Bottom;
		textBG->GetWidgetProps().dropshadow.steps		  = 12;
		textBG->GetWidgetProps().dropshadow.margin		  = Theme::GetDef().baseOutlineThickness;
		textBG->GetWidgetProps().interpolateColor		  = true;
		textBG->GetWidgetProps().colorInterpolateSpeed	  = 20.0f;
		textBG->GetWidgetProps().colorBackgroundDirection = DirectionOrientation::Vertical;

		textBG->GetWidgetProps().rounding		  = 0.0f;
		textBG->GetWidgetProps().outlineThickness = 0.0f;
		layout->AddChild(textBG);

		Text* title = wm->Allocate<Text>("Title");
		title->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		title->SetAlignedPos(Vector2(0.5f, 0.5f));
		title->SetAnchorX(Anchor::Center);
		title->SetAnchorY(Anchor::Center);
		title->GetProps().text = item->name;
		//	title->GetProps().fetchWrapFromParent		= true;
		//	title->GetProps().wordWrap					= false;
		title->GetProps().fetchCustomClipFromParent = true;
		title->GetProps().isDynamic					= true;
		title->SetUserData(item);
		textBG->AddChild(title);
		layout->Initialize();
		return layout;
	}

	GenericPopup* CommonWidgets::ThrowGenericPopup(const String& title, const String& text, const String& icon, Widget* source)
	{
		GenericPopup* pp		  = source->GetWidgetManager()->Allocate<GenericPopup>("GenericPopup");
		pp->GetPopupProps().text  = text;
		pp->GetPopupProps().title = title;
		pp->GetPopupProps().icon  = icon;
		pp->Initialize();
		return pp;
	}

	Widget* CommonWidgets::BuildFieldLayout(Widget* src, const String& title)
	{
		WidgetManager* wm = src->GetWidgetManager();

		DirectionalLayout* layout = wm->Allocate<DirectionalLayout>("FieldLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->SetAlignedPosX(0.0f);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;

		Text* txt			 = wm->Allocate<Text>("FieldTitle");
		txt->GetProps().text = title;
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		layout->AddChild(txt);

		return layout;
	}

	Widget* CommonWidgets::BuildField(Widget* src, const String& title, StringID fieldType, FieldValue reflectionValue, FieldBase* field)
	{
		WidgetManager* wm	  = src->GetWidgetManager();
		Widget*		   layout = BuildFieldLayout(src, title);

		DirectionalLayout* rightSide = wm->Allocate<DirectionalLayout>("RightSide");
		rightSide->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_ALIGN_X);
		rightSide->GetProps().direction			 = DirectionOrientation::Horizontal;
		rightSide->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		rightSide->SetAlignedPosX(1.0f);
		rightSide->SetAlignedPosY(0.0f);
		rightSide->SetAlignedSizeX(0.5f);
		rightSide->SetAlignedSizeY(1.0f);
		rightSide->GetProps().mode = DirectionalLayout::Mode::EqualSizes;
		rightSide->SetAnchorX(Anchor::End);
		layout->AddChild(rightSide);

		auto getValueField = [wm](float* ptr, bool hasLimits, float minFloat, float maxFloat) -> InputField* {
			InputField* inp = wm->Allocate<InputField>();
			inp->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			inp->SetAlignedSize(Vector2(0.0f, 1.0f));
			inp->SetAlignedPosY(0.0f);
			inp->GetText()->GetProps().text		= TO_STRING(*ptr);
			inp->GetProps().value				= ptr;
			inp->GetProps().isNumberField		= true;
			inp->GetProps().disableNumberSlider = !hasLimits;
			inp->GetProps().valueMin			= minFloat;
			inp->GetProps().valueMax			= maxFloat;
			inp->GetProps().valueStep			= (maxFloat - minFloat) / 20.0f;
			return inp;
		};

		String min = "", max = "";
		if (field->HasProperty<String>("Min"_hs))
		{
			min = field->GetProperty<String>("Min"_hs);
			max = field->GetProperty<String>("Max"_hs);
		}

		if (fieldType == "Bitmask32"_hs)
		{
			Dropdown* dd = wm->Allocate<Dropdown>();
			dd->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			dd->SetAlignedSize(Vector2(0.0f, 1.0f));
			dd->SetAlignedPosY(0.0f);

			Bitmask32 bm				   = reflectionValue.GetValue<Bitmask32>();
			dd->GetText()->GetProps().text = TO_STRING(bm.GetValue());

			rightSide->AddChild(dd);
		}
		else if (fieldType == "enum"_hs)
		{
			Dropdown* dd = wm->Allocate<Dropdown>();
			dd->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
			dd->SetAlignedSize(Vector2(0.0f, 1.0f));
			dd->SetAlignedPosY(0.0f);

			int32* enumVal = reflectionValue.CastPtr<int32>();

			dd->GetProps().onSelected = [enumVal](int32 item) -> bool {
				*enumVal = item;
				return true;
			};

			rightSide->AddChild(dd);
		}
		else if (fieldType == "Vector2"_hs)
		{
			Vector2* val = reflectionValue.CastPtr<Vector2>();

			uint32 outDecimals = 0;

			const bool hasLimits = !min.empty();
			float	   minFloat = 0.0f, maxFloat = 0.0f;

			if (hasLimits)
			{
				minFloat = UtilStr::StringToFloat(min, outDecimals);
				maxFloat = UtilStr::StringToFloat(max, outDecimals);
			}

			rightSide->AddChild(getValueField(&val->x, hasLimits, minFloat, maxFloat));
			rightSide->AddChild(getValueField(&val->y, hasLimits, minFloat, maxFloat));
		}
		else if (fieldType == "Vector3"_hs)
		{
			Vector3* val = reflectionValue.CastPtr<Vector3>();

			uint32 outDecimals = 0;

			const bool hasLimits = !min.empty();
			float	   minFloat = 0.0f, maxFloat = 0.0f;

			if (hasLimits)
			{
				minFloat = UtilStr::StringToFloat(min, outDecimals);
				maxFloat = UtilStr::StringToFloat(max, outDecimals);
			}

			rightSide->AddChild(getValueField(&val->x, hasLimits, minFloat, maxFloat));
			rightSide->AddChild(getValueField(&val->y, hasLimits, minFloat, maxFloat));
			rightSide->AddChild(getValueField(&val->z, hasLimits, minFloat, maxFloat));
		}
		else if (fieldType == "Vector4"_hs)
		{
			Vector4* val = reflectionValue.CastPtr<Vector4>();

			uint32 outDecimals = 0;

			const bool hasLimits = !min.empty();
			float	   minFloat = 0.0f, maxFloat = 0.0f;

			if (hasLimits)
			{
				minFloat = UtilStr::StringToFloat(min, outDecimals);
				maxFloat = UtilStr::StringToFloat(max, outDecimals);
			}

			rightSide->AddChild(getValueField(&val->x, hasLimits, minFloat, maxFloat));
			rightSide->AddChild(getValueField(&val->y, hasLimits, minFloat, maxFloat));
			rightSide->AddChild(getValueField(&val->y, hasLimits, minFloat, maxFloat));
			rightSide->AddChild(getValueField(&val->w, hasLimits, minFloat, maxFloat));
		}

		layout->Initialize();
		return layout;
	}

} // namespace Lina::Editor
