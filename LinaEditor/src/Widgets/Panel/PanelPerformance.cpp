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

#include "Editor/Widgets/Panel/PanelPerformance.hpp"
#include "Editor/Widgets/Compound/IconTabs.hpp"
#include "Editor/Widgets/Compound/Table.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina::Editor
{

	void PanelPerformance::Construct()
	{
		m_editor = Editor::Get();

		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		AddChild(layout);

		IconTabs* iconTabs = m_manager->Allocate<IconTabs>("IconTabs");
		iconTabs->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
		iconTabs->SetAlignedPosY(0.0f);
		iconTabs->SetAlignedSizeY(1.0f);
		iconTabs->SetFixedSizeX(Theme::GetDef().baseItemHeight * 2);
		iconTabs->GetProps().icons = {
			{
				.color	 = Theme::GetDef().foreground1,
				.icon	 = ICON_GAUGE,
				.tooltip = Locale::GetStr(LocaleStr::Profiling),
			},
			{
				.color	 = Theme::GetDef().foreground1,
				.icon	 = ICON_MEMORY,
				.tooltip = Locale::GetStr(LocaleStr::Memory),
			},
			{
				.color	 = Theme::GetDef().foreground1,
				.icon	 = ICON_CUBE,
				.tooltip = Locale::GetStr(LocaleStr::Resources),
			},
		};
		iconTabs->GetWidgetProps().borderThickness.right = Theme::GetDef().baseSeparatorThickness;
		iconTabs->GetWidgetProps().colorBorders			 = Theme::GetDef().background0;
		iconTabs->GetProps().onSelected					 = [this](int32 idx) {
			 m_editor->GetSettings().GetSettingsPanelStats().selectedTab = idx;
			 m_editor->SaveSettings();
			 SelectContent(idx);
		};
		iconTabs->GetWidgetProps().colorBackground	  = Theme::GetDef().background2;
		iconTabs->GetWidgetProps().colorBackgroundAlt = Theme::GetDef().background1;
		m_iconTabs									  = iconTabs;
		layout->AddChild(iconTabs);

		DirectionalLayout* contents	   = m_manager->Allocate<DirectionalLayout>("Contents");
		contents->GetProps().direction = DirectionOrientation::Vertical;
		contents->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		contents->SetAlignedPosY(0.0f);
		contents->SetAlignedSize(Vector2(0.0f, 1.0f));
		contents->GetWidgetProps().childMargins.top	   = Theme::GetDef().baseIndent;
		contents->GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndent;
		contents->GetWidgetProps().childPadding		   = Theme::GetDef().baseIndent;
		layout->AddChild(contents);
		m_layout = contents;

		SelectContent(m_editor->GetSettings().GetSettingsPanelStats().selectedTab);
	}

	void PanelPerformance::Destruct()
	{
		Panel::Destruct();
	}

	void PanelPerformance::SelectContent(int32 index)
	{
		m_iconTabs->SetSelected(index);

		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();

		if (index == 0)
			BuildContentsProfiling();
		else if (index == 1)
			BuildContentsMemory();
		else
			BuildContentsResources();
	}

	void PanelPerformance::BuildContentsProfiling()
	{
		auto wrap = [&](Widget* child) -> DirectionalLayout* {
			DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Wrap");
			layout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
			layout->SetAlignedPosX(0.0f);
			layout->SetAlignedSizeX(1.0f);
			layout->SetFixedSizeY(Theme::GetDef().baseItemHeight);
			layout->GetProps().direction				= DirectionOrientation::Horizontal;
			layout->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
			layout->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
			child->GetFlags().Set(WF_POS_ALIGN_Y);
			child->SetAlignedPosY(0.6f);
			child->SetAnchorY(Anchor::Center);
			layout->AddChild(child);
			return layout;
		};

		Text* frameTime = m_manager->Allocate<Text>("FrameTime");

		frameTime->GetProps().isDynamic = true;
		frameTime->SetTickHook([frameTime](float delta) {
			static float elapsed = 1.0f;

			elapsed += delta;

			if (elapsed > 1.0f)
			{
				const float	 fps = 1.0f / delta;
				const String str = UtilStr::FloatToString(delta, 8) + " ms - " + TO_STRING(static_cast<int32>(fps)) + " (FPS)";
				frameTime->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::FrameTime) + ": " + str);
				elapsed = 0.0f;
			}
		});
		m_layout->AddChild(wrap(frameTime));

		Table* table = m_manager->Allocate<Table>("Table");
		table->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		table->SetAlignedPosX(0.0f);
		table->SetAlignedSize(Vector2(1.0f, 0.0f));
		table->GetProps().columns = {
			{
				.title = "Test",
			},
			{
				.title = "Test2",
			},
			{
				.title = "Test2",
			},

		};
		m_layout->AddChild(table);
	}

	void PanelPerformance::BuildContentsMemory()
	{
	}

	void PanelPerformance::BuildContentsResources()
	{
	}

} // namespace Lina::Editor
