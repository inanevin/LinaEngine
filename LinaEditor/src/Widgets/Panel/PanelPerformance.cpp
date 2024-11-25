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
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Graph/LineGraph.hpp"
#include "Editor/Widgets/Graph/CircleGauge.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Core/Application.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Circle.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina::Editor
{
#define RECORD_LAST_N_FRAMES 200

	void PanelPerformance::Construct()
	{
		m_editor = Editor::Get();

		const uint8 selectedTab	  = m_editor->GetSettings().GetParams().GetParamUint8("panelPerformanceTab"_hs, 0);
		const uint8 resourcesSort = m_editor->GetSettings().GetParams().GetParamUint8("panelPerformanceResSort"_hs, 0);

		m_resourcesData.sort = static_cast<ResourcesSort>(resourcesSort);

		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		AddChild(layout);

		IconTabs* iconTabs = m_manager->Allocate<IconTabs>("IconTabs");
		iconTabs->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
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
				.icon	 = ICON_CUBE,
				.tooltip = Locale::GetStr(LocaleStr::Resources),
			},
		};

		iconTabs->SetFixedSizeY(iconTabs->GetFixedSizeX() * static_cast<float>(iconTabs->GetProps().icons.size()));

		iconTabs->GetProps().onSelected = [this](uint8 idx) {
			m_editor->GetSettings().GetParams().SetParamUint8("panelPerformanceTab"_hs, idx);
			m_editor->SaveSettings();
			SelectContent(idx);
		};
		iconTabs->GetWidgetProps().colorBackground	  = Theme::GetDef().background2;
		iconTabs->GetWidgetProps().colorBackgroundAlt = Theme::GetDef().background1;
		m_iconTabs									  = iconTabs;
		layout->AddChild(iconTabs);

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPosY(0.0f);
		scroll->SetAlignedSize(Vector2(0.0f, 1.0f));
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		layout->AddChild(scroll);

		m_layout					   = m_manager->Allocate<DirectionalLayout>("Contents");
		m_layout->GetProps().direction = DirectionOrientation::Vertical;
		m_layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_layout->SetAlignedPos(Vector2::Zero);
		m_layout->SetAlignedSize(Vector2::One);
		m_layout->GetWidgetProps().clipChildren			= true;
		m_layout->GetWidgetProps().borderThickness.left = Theme::GetDef().baseSeparatorThickness;
		m_layout->GetWidgetProps().colorBorders			= Theme::GetDef().background0;
		m_layout->GetWidgetProps().childMargins.top		= Theme::GetDef().baseIndent;
		m_layout->GetWidgetProps().childMargins.bottom	= Theme::GetDef().baseIndent;
		m_layout->GetWidgetProps().childPadding			= Theme::GetDef().baseIndent;
		scroll->AddChild(m_layout);
		scroll->SetTarget(m_layout);
		scroll->SetDisplayTarget(m_layout);

		SelectContent(selectedTab);

		m_editor->GetApp()->GetResourceManager().AddListener(this);
	}

	void PanelPerformance::Destruct()
	{
		m_editor->GetApp()->GetResourceManager().RemoveListener(this);
		Panel::Destruct();
	}

	void PanelPerformance::PreTick()
	{
		if (m_currentContent == 0)
		{

#ifdef LINA_ENABLE_PROFILER
			const Profiler::DrawInfo& inf = Profiler::Get()->GetDrawInfo();
			m_profilingData._tris		  = inf.tris;
			m_profilingData._verts		  = inf.vertices;
			m_profilingData._indices	  = inf.indices;
			m_profilingData._dc			  = inf.drawCalls;
#endif
		}
	}

	void PanelPerformance::Tick(float delta)
	{

		if (m_currentContent == 0)
		{
			static float cpuUtilTimer = 0.0f;
			cpuUtilTimer += delta;
			if (cpuUtilTimer > .5f)
			{
				const float utilization									 = Math::Clamp(PlatformProcess::GetCPULoad(), 0.0f, 1.0f);
				cpuUtilTimer											 = 0.0f;
				m_profilingData.cpuUtilCircle->GetProps().foregroundFill = utilization;
				m_profilingData.cpuUtilText->UpdateTextAndCalcSize(UtilStr::FloatToString(utilization * 100.0f, 0) + " %");
			}

			static float memTimer = 0.0f;
			memTimer += delta;
			if (memTimer > 1.0f)
			{
				memTimer = 0.0f;

				const PlatformProcess::MemoryInformation info			 = PlatformProcess::QueryMemInformation();
				m_profilingData.memCircle->GetProps().foregroundFill	 = static_cast<float>(info.currentUsage) / static_cast<float>(info.totalMemory);
				m_profilingData.memPeakCircle->GetProps().foregroundFill = static_cast<float>(info.peakUsage) / static_cast<float>(info.totalMemory);

				const String currentUsageStr = UtilStr::FloatToString(static_cast<float>(info.currentUsage) / (1024 * 1024), 0);
				const String peakUsageStr	 = UtilStr::FloatToString(static_cast<float>(info.peakUsage) / (1024 * 1024), 0);
				m_profilingData.memText->UpdateTextAndCalcSize(currentUsageStr + " MB");
				m_profilingData.memPeakText->UpdateTextAndCalcSize(peakUsageStr + " MB");
			}

			static float tableTimer = 0.0f;
			tableTimer += delta;

			if (tableTimer > .5f)
			{
				tableTimer = 0.0f;

				m_profilingData.delta->UpdateTextAndCalcSize(UtilStr::FloatToString(delta, 10));
				m_profilingData.smoothedDelta->UpdateTextAndCalcSize(UtilStr::FloatToString(SystemInfo::GetSmoothedDeltaTime(), 10));
				m_profilingData.dpi->UpdateTextAndCalcSize(UtilStr::FloatToString(m_lgxWindow->GetDPIScale(), 1));
				m_profilingData.tris->UpdateTextAndCalcSize(TO_STRING(m_profilingData._tris));
				m_profilingData.vertices->UpdateTextAndCalcSize(TO_STRING(m_profilingData._verts));
				m_profilingData.indices->UpdateTextAndCalcSize(TO_STRING(m_profilingData._indices));
				m_profilingData.drawCalls->UpdateTextAndCalcSize(TO_STRING(m_profilingData._dc));
			}

			LineGraph::PointGroup& group = m_profilingData.fpsGraph->GetProps().groups.back();
			// group.points.resize(RECORD);

			LineGraph::Legend& legend = m_profilingData.fpsGraph->GetProps().legends.back();
			legend.text				  = "FPS: " + UtilStr::FloatToString(1.0f / delta, 0) + " MS: " + UtilStr::FloatToString(delta * 1000.0f, 2);

			m_profilingData.fpsFrames.push_back(delta);

			if (m_profilingData.fpsFrames.size() > RECORD_LAST_N_FRAMES)
				m_profilingData.fpsFrames.pop_front();

			static float lastTime = 0.0f;

			lastTime += delta;

			if (lastTime > 0.01f && m_profilingData.fpsFrames.size() == RECORD_LAST_N_FRAMES)
			{
				lastTime   = 0.0f;
				uint32 idx = 0;

				for (float f : m_profilingData.fpsFrames)
				{
					group.points[idx] = {
						.crossAxisValue = 1.0f / f,
						.mainAxisValue	= static_cast<float>(idx),
					};
					idx++;
				}
			}
		}
	}

	void PanelPerformance::OnResourceManagerPreDestroyHW(const HashSet<Resource*>& resources)
	{
		if (m_iconTabs->GetSelected() != 2)
			return;

		RefreshResourcesTable();
	}

	void PanelPerformance::OnResourceManagerGeneratedHW(const HashSet<Resource*>& resources)
	{
		if (m_iconTabs->GetSelected() != 2)
			return;

		RefreshResourcesTable();
	}

	void PanelPerformance::SaveLayoutToStream(OStream& stream)
	{

		if (m_resourcesData.table != nullptr)
		{
			const Vector<Widget*> children = m_resourcesData.table->GetHorizontalLayout()->GetChildren();
			const uint32		  sz	   = static_cast<uint32>(children.size());
			stream << sz;
			for (Widget* w : children)
			{
				stream << w->GetAlignedPosX();
				stream << w->GetAlignedSizeX();
			}
		}
		else
			stream << 0;

		if (m_profilingData.infoTable != nullptr)
		{
			const Vector<Widget*> children = m_profilingData.infoTable->GetHorizontalLayout()->GetChildren();
			const uint32		  sz	   = static_cast<uint32>(children.size());
			stream << sz;
			for (Widget* w : children)
			{
				stream << w->GetAlignedPosX();
				stream << w->GetAlignedSizeX();
			}
		}
		else
			stream << 0;
	}

	void PanelPerformance::LoadLayoutFromStream(IStream& stream)
	{
		uint32 szResTable = 0;
		stream >> szResTable;

		for (uint32 i = 0; i < szResTable; i++)
		{
			float posX = 0.0f, sizeX = 0.0f;
			stream >> posX >> sizeX;
			if (m_resourcesData.table != nullptr)
			{
				const Vector<Widget*>& children = m_resourcesData.table->GetHorizontalLayout()->GetChildren();
				if (i < static_cast<uint32>(children.size()))
				{
					children[i]->SetAlignedPosX(posX);
					children[i]->SetAlignedSizeX(sizeX);
				}
			}
		}

		uint32 szInfoTable = 0;
		stream >> szInfoTable;

		for (uint32 i = 0; i < szInfoTable; i++)
		{
			float posX = 0.0f, sizeX = 0.0f;
			stream >> posX >> sizeX;
			if (m_profilingData.infoTable != nullptr)
			{
				const Vector<Widget*>& children = m_profilingData.infoTable->GetHorizontalLayout()->GetChildren();
				if (i < static_cast<uint32>(children.size()))
				{
					children[i]->SetAlignedPosX(posX);
					children[i]->SetAlignedSizeX(sizeX);
				}
			}
		}
	}

	void PanelPerformance::SelectContent(uint8 idx)
	{
		m_iconTabs->SetSelected(idx);

		m_layout->DeallocAllChildren();
		m_layout->RemoveAllChildren();

		m_profilingData = {};
		m_resourcesData = {};

		m_currentContent = idx;

		if (idx == 0)
			BuildContentsProfiling();
		else
			BuildContentsResources();
	}

	void PanelPerformance::BuildContentsProfiling()
	{
		m_layout->GetWidgetProps().childMargins.left  = Theme::GetDef().baseIndent;
		m_layout->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;

		DirectionalLayout* fpsGraphLayout = m_manager->Allocate<DirectionalLayout>("FPSGraphLayout");
		fpsGraphLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		fpsGraphLayout->SetAlignedPosX(0.0f);
		fpsGraphLayout->SetAlignedSizeX(1.0f);
		fpsGraphLayout->SetAlignedSizeY(1.0f);
		fpsGraphLayout->GetWidgetProps().childMargins	  = TBLR::Eq(Theme::GetDef().baseIndent);
		fpsGraphLayout->GetWidgetProps().childPadding	  = Theme::GetDef().baseIndent;
		fpsGraphLayout->GetProps().direction			  = DirectionOrientation::Vertical;
		fpsGraphLayout->GetWidgetProps().drawBackground	  = true;
		fpsGraphLayout->GetWidgetProps().colorBackground  = Theme::GetDef().background1;
		fpsGraphLayout->GetWidgetProps().rounding		  = 0.02f;
		fpsGraphLayout->GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		fpsGraphLayout->GetWidgetProps().colorOutline	  = Theme::GetDef().background0;
		m_layout->AddChild(fpsGraphLayout);

		m_profilingData.fpsGraph = m_manager->Allocate<LineGraph>("Graph");
		m_profilingData.fpsGraph->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		m_profilingData.fpsGraph->SetAlignedPosX(0.0f);
		m_profilingData.fpsGraph->SetAlignedSizeX(1.0f);
		m_profilingData.fpsGraph->SetFixedSizeY(Theme::GetDef().baseItemHeight * 12);

		m_profilingData.fpsGraph->GetProps().drawGridBackground = true;
		m_profilingData.fpsGraph->GetProps().colorBackground	= Theme::GetDef().background0;
		m_profilingData.fpsGraph->GetProps().colorBarMainAxis	= ColorGrad(Theme::GetDef().accentPrimary0, Theme::GetDef().accentSecondary);
		m_profilingData.fpsGraph->GetProps().colorBarCrossAxis	= ColorGrad(Theme::GetDef().accentSecondary, Theme::GetDef().accentPrimary0);

		m_profilingData.fpsGraph->GetProps().gridCells.y		 = 5;
		m_profilingData.fpsGraph->GetProps().colorGridHorizontal = Theme::GetDef().silent0;
		m_profilingData.fpsGraph->GetProps().crossAxisDynamic	 = true;
		m_profilingData.fpsGraph->GetProps().colorTextMainAxis	 = Theme::GetDef().accentSecondary;
		m_profilingData.fpsGraph->GetProps().colorTextCrossAxis	 = Theme::GetDef().accentPrimary1;

		m_profilingData.fpsGraph->GetProps().legends = {
			{
				.text  = "FPS",
				.color = ColorGrad(Theme::GetDef().accentPrimary0, Theme::GetDef().accentSecondary),
			},
		};

		m_profilingData.fpsGraph->GetProps().groups = {
			{
				.colorLine = ColorGrad(Theme::GetDef().accentPrimary0, Theme::GetDef().accentSecondary),
				.drawLine  = true,
			},
		};

		m_profilingData.fpsGraph->GetProps().groups.back().points.resize(RECORD_LAST_N_FRAMES);
		m_profilingData.fpsGraph->GetProps().groups.back().thickness = Theme::GetDef().baseOutlineThickness * 2;

		m_profilingData.fpsGraph->GetProps().mainAxisPoints = {
			{
				.value		 = 0.0f,
				.displayName = "",
			},
			{
				.value		 = 25.0f,
				.displayName = "N25",
			},
			{
				.value		 = 100.0f,
				.displayName = "N100",
			},
			{
				.value		 = 200.0f,
				.displayName = "N200",
			},
		};

		fpsGraphLayout->AddChild(m_profilingData.fpsGraph);

		Text* fpsGraphTitle = m_manager->Allocate<Text>("FPSGraphTitle");
		fpsGraphTitle->GetFlags().Set(WF_POS_ALIGN_X);
		fpsGraphTitle->GetProps().font = EDITOR_FONT_PLAY_BIG_ID;
		fpsGraphTitle->SetAlignedPosX(0.5f);
		fpsGraphTitle->SetAnchorX(Anchor::Center);
		fpsGraphTitle->UpdateTextAndCalcSize("FPS/Frames");
		fpsGraphLayout->AddChild(fpsGraphTitle);

		DirectionalLayout* horizontal = m_manager->Allocate<DirectionalLayout>("Horizontal");
		horizontal->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		horizontal->SetAlignedPosX(0.0f);
		horizontal->SetAlignedSizeX(1.0f);
		horizontal->SetFixedSizeY(Theme::GetDef().baseItemHeight * 8);
		horizontal->GetWidgetProps().childPadding = Theme::GetDef().baseIndent;
		horizontal->GetProps().direction		  = DirectionOrientation::Horizontal;
		m_layout->AddChild(horizontal);

		auto createGauge = [this]() -> CircleGauge* {
			CircleGauge* gauge = m_manager->Allocate<CircleGauge>("Gauge");
			gauge->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
			gauge->SetAlignedPosY(0.0f);
			gauge->SetAlignedSizeY(1.0f);
			gauge->SetFixedSizeX(Theme::GetDef().baseItemHeight * 10);
			gauge->GetWidgetProps().childMargins	 = TBLR::Eq(Theme::GetDef().baseIndent);
			gauge->GetWidgetProps().rounding		 = 0.05f;
			gauge->GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
			gauge->GetWidgetProps().colorOutline	 = Theme::GetDef().background0;
			gauge->GetWidgetProps().drawBackground	 = true;
			gauge->GetWidgetProps().colorBackground	 = Theme::GetDef().background1;

			gauge->GetCircle()->GetProps().startAngle	   = 180.0f;
			gauge->GetCircle()->GetProps().endAngle		   = 360.0f;
			gauge->GetCircle()->GetProps().isFilled		   = false;
			gauge->GetCircle()->GetProps().useAA		   = true;
			gauge->GetCircle()->GetProps().thickness	   = Theme::GetDef().baseItemHeight * 0.25f;
			gauge->GetCircle()->GetProps().colorBackground = Theme::GetDef().background0;
			gauge->GetCircle()->GetProps().foregroundFill  = 0.5f;
			gauge->GetCircle()->GetProps().useXForRadius   = true;
			return gauge;
		};

		// CPU gauge
		{
			CircleGauge* gauge = createGauge();

			m_profilingData.cpuUtilCircle							  = gauge->GetCircle();
			m_profilingData.cpuUtilCircle->GetProps().colorForeground = ColorGrad(Theme::GetDef().accentSecondary, Theme::GetDef().accentPrimary0);
			m_profilingData.cpuUtilText								  = gauge->GetDataText();
			m_profilingData.cpuUtilText->GetProps().font			  = EDITOR_FONT_PLAY_VERY_BIG_ID;
			m_profilingData.cpuUtilText->GetProps().color			  = ColorGrad(Theme::GetDef().accentSecondary, Theme::GetDef().accentPrimary0);
			m_profilingData.cpuUtilText->UpdateTextAndCalcSize("25%");

			gauge->GetTitle()->GetProps().font = EDITOR_FONT_PLAY_BIG_ID;
			gauge->GetTitle()->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::CPUUtilization));

			horizontal->AddChild(gauge);
		}

		// Memory gauge
		{
			CircleGauge* gauge = createGauge();

			m_profilingData.memCircle							  = gauge->GetCircle();
			m_profilingData.memCircle->GetProps().startAngle	  = 180.0f;
			m_profilingData.memCircle->GetProps().colorForeground = ColorGrad(Theme::GetDef().accentSuccess, Theme::GetDef().accentOrange);
			m_profilingData.memText								  = gauge->GetDataText();
			m_profilingData.memText->GetProps().font			  = EDITOR_FONT_PLAY_VERY_BIG_ID;
			m_profilingData.memText->GetProps().color			  = ColorGrad(Theme::GetDef().accentSuccess, Theme::GetDef().accentOrange);
			m_profilingData.memText->UpdateTextAndCalcSize("0 MB");

			gauge->GetTitle()->GetProps().font = EDITOR_FONT_PLAY_BIG_ID;
			gauge->GetTitle()->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::Memory));

			horizontal->AddChild(gauge);
		}

		// Peak memory gauge
		{
			CircleGauge* gauge = createGauge();

			m_profilingData.memPeakCircle							  = gauge->GetCircle();
			m_profilingData.memPeakCircle->GetProps().colorForeground = ColorGrad(Theme::GetDef().accentOrange, Theme::GetDef().accentError);
			m_profilingData.memPeakText								  = gauge->GetDataText();
			m_profilingData.memPeakText->GetProps().font			  = EDITOR_FONT_PLAY_VERY_BIG_ID;
			m_profilingData.memPeakText->GetProps().color			  = ColorGrad(Theme::GetDef().accentOrange, Theme::GetDef().accentError);
			m_profilingData.memPeakText->UpdateTextAndCalcSize("0 MB");

			gauge->GetTitle()->GetProps().font = EDITOR_FONT_PLAY_BIG_ID;
			gauge->GetTitle()->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::PeakMemory));

			horizontal->AddChild(gauge);
		}

		DirectionalLayout* info = m_manager->Allocate<DirectionalLayout>("InfoLayout");
		info->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_ALIGN_X);
		info->SetAlignedPosY(0.0f);
		info->SetAlignedSize(Vector2(0.0f, 1.0f));
		info->GetProps().direction				= DirectionOrientation::Vertical;
		info->GetWidgetProps().drawBackground	= true;
		info->GetWidgetProps().outlineThickness = 0.0f;
		info->GetWidgetProps().colorBackground	= Theme::GetDef().background1;
		info->GetWidgetProps().rounding			= 0.05f;
		info->GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness;
		info->GetWidgetProps().colorOutline		= Theme::GetDef().background0;
		horizontal->AddChild(info);

		m_profilingData.infoTable = m_manager->Allocate<Table>("m_profilingData.infoTable");
		m_profilingData.infoTable->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_profilingData.infoTable->SetAlignedPos(Vector2::Zero);
		m_profilingData.infoTable->SetAlignedSize(Vector2::One);
		m_profilingData.infoTable->GetProps().displayHeaders = false;
		m_profilingData.infoTable->GetProps().clipChildren	 = false;

		m_profilingData.infoTable->BuildHeaders({
			{
				.text	   = "Prop",
				.clickable = false,
			},
			{
				.text	   = "Value",
				.clickable = false,
			},
		});

		auto buildText = [&](const String& txt) -> Text* {
			Text* text = m_manager->Allocate<Text>("");
			text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			text->SetAlignedPos(Vector2(0.0f, 0.5f));
			text->SetAnchorX(Anchor::Start);
			text->SetAnchorY(Anchor::Center);
			text->UpdateTextAndCalcSize(txt);
			text->GetProps().fetchCustomClipFromParent = true;
			text->GetProps().isDynamic				   = true;
			return text;
		};

		m_profilingData.tris		  = buildText("0");
		m_profilingData.vertices	  = buildText("0");
		m_profilingData.indices		  = buildText("0");
		m_profilingData.drawCalls	  = buildText("0");
		m_profilingData.dpi			  = buildText("0");
		m_profilingData.delta		  = buildText("0.0f");
		m_profilingData.smoothedDelta = buildText("0.0f");

		m_profilingData.infoTable->AddRow({buildText(Locale::GetStr(LocaleStr::Delta)), m_profilingData.delta});
		m_profilingData.infoTable->AddRow({buildText(Locale::GetStr(LocaleStr::SmoothedDelta)), m_profilingData.smoothedDelta});
		m_profilingData.infoTable->AddRow({buildText(Locale::GetStr(LocaleStr::DPIScale)), m_profilingData.dpi});
		m_profilingData.infoTable->AddRow({buildText(Locale::GetStr(LocaleStr::Vertices)), m_profilingData.vertices});
		m_profilingData.infoTable->AddRow({buildText(Locale::GetStr(LocaleStr::Indices)), m_profilingData.indices});
		m_profilingData.infoTable->AddRow({buildText(Locale::GetStr(LocaleStr::Tris)), m_profilingData.tris});
		m_profilingData.infoTable->AddRow({buildText(Locale::GetStr(LocaleStr::DrawCalls)), m_profilingData.drawCalls});
		m_profilingData.infoTable->GetHorizontalLayout()->GetProps().onBordersChanged = [this]() { m_editor->SaveSettings(); };

		info->AddChild(m_profilingData.infoTable);
	}

	void PanelPerformance::BuildContentsResources()
	{
		m_layout->GetWidgetProps().childMargins.left  = 0.0f;
		m_layout->GetWidgetProps().childMargins.right = 0.0f;

		DirectionalLayout* header = m_manager->Allocate<DirectionalLayout>("Header");
		header->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		header->SetAlignedPosX(0.0f);
		header->SetAlignedSizeX(1.0f);
		header->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		header->GetProps().direction				= DirectionOrientation::Horizontal;
		header->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
		header->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
		header->GetWidgetProps().childPadding		= Theme::GetDef().baseIndent;
		m_layout->AddChild(header);

		InputField* search = m_manager->Allocate<InputField>("Search");
		search->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		search->SetAlignedPosY(0.0f);
		search->SetAlignedSize(Vector2(0.0f, 1.0f));
		search->GetProps().placeHolderIcon = ICON_SEARCH;
		search->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search);
		search->GetProps().onEdited		   = [this](const String& str) {
			   m_resourcesData.searchStr = UtilStr::ToLower(str);
			   RefreshResourcesTable();
		};

		header->AddChild(search);

		m_resourcesData.table													  = m_manager->Allocate<Table>("Table");
		m_resourcesData.table->GetHorizontalLayout()->GetProps().onBordersChanged = [this]() { m_editor->SaveSettings(); };
		m_resourcesData.table->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_resourcesData.table->SetAlignedPosX(0.0f);
		m_resourcesData.table->SetAlignedSize(Vector2(1.0f, 0.0f));
		m_resourcesData.table->GetHorizontalLayout()->GetProps().borderMinSize = 0.08f;
		m_resourcesData.table->BuildHeaders({
			{
				.text	   = Locale::GetStr(LocaleStr::Name),
				.clickable = true,
			},
			{
				.text	   = Locale::GetStr(LocaleStr::ID),
				.clickable = true,
			},
			{
				.text	   = Locale::GetStr(LocaleStr::Type),
				.clickable = true,
			},
			{
				.text	   = Locale::GetStr(LocaleStr::Preview),
				.clickable = false,
			},
			{
				.text	   = Locale::GetStr(LocaleStr::SizeHW),
				.clickable = true,
			},
		});

		m_resourcesData.table->GetProps().onColumnClicked = [this](uint32 idx) {
			const ResourcesSort prev = m_resourcesData.sort;

			if (idx == 0)
				m_resourcesData.sort = ResourcesSort::Name;
			if (idx == 1)
				m_resourcesData.sort = ResourcesSort::ID;
			if (idx == 2)
				m_resourcesData.sort = ResourcesSort::Type;
			if (idx == 4)
				m_resourcesData.sort = ResourcesSort::Size;

			if (prev != m_resourcesData.sort)
			{
				m_editor->GetSettings().GetParams().SetParamUint8("panelPerformanceResSort"_hs, static_cast<uint8>(m_resourcesData.sort));
				m_editor->SaveSettings();
				RefreshResourcesTable();
			}
		};
		m_layout->AddChild(m_resourcesData.table);
		RefreshResourcesTable();
	}

	void PanelPerformance::RefreshResourcesTable()
	{
		m_resourcesData.table->ClearRows();

		ResourceManagerV2& rm = m_editor->GetApp()->GetResourceManager();

		const Vector<ResourceManagerV2::CachePair>& caches = rm.GetCaches();

		struct Row
		{
			String	   displayName = "";
			ResourceID id		   = 0;
			TypeID	   type		   = 0;
			size_t	   size		   = 0;
		};

		Vector<Row> rows;
		rows.reserve(100);

		for (const ResourceManagerV2::CachePair& pair : caches)
		{
			const TypeID			tid		  = pair.tid;
			const Vector<Resource*> resources = pair.cache->GetAllResources();
			for (Resource* res : resources)
			{
				const String name = res->GetName().empty() ? res->GetPath() : res->GetName();

				if (!m_resourcesData.searchStr.empty() && UtilStr::ToLower(name).find(m_resourcesData.searchStr) == String::npos)
					continue;

				const Row row = {
					.displayName = name,
					.id			 = res->GetID(),
					.type		 = res->GetTID(),
					.size		 = res->GetSize(),
				};

				rows.push_back(row);
			}
		}

		linatl::sort(rows.begin(), rows.end(), [this](const Row& r1, const Row& r2) -> bool {
			if (m_resourcesData.sort == ResourcesSort::Name)
				return r1.displayName < r2.displayName;

			if (m_resourcesData.sort == ResourcesSort::ID)
				return r1.id < r2.id;

			if (m_resourcesData.sort == ResourcesSort::Type)
				return r1.type < r2.type;

			if (m_resourcesData.sort == ResourcesSort::Size)
				return r1.size < r2.size;

			return false;
		});

		Vector<Widget*> widgets;
		widgets.resize(5);

		auto createText = [&](const String& title, const Color& textColor) -> Widget* {
			Text* txt = m_manager->Allocate<Text>("Text");
			txt->UpdateTextAndCalcSize(title);
			txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			txt->SetAlignedPos(Vector2(0.0f, 0.5f));
			txt->SetAnchorY(Anchor::Center);
			txt->GetProps().color					  = textColor;
			txt->GetProps().fetchCustomClipFromParent = true;
			txt->GetProps().isDynamic				  = true;
			return txt;
		};

		auto createVis = [&](TextureAtlasImage* img) -> Widget* {
			Widget* txt = m_manager->Allocate<Widget>("Txt");
			txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y);
			txt->SetAlignedPos(Vector2(0.0f, 0.5f));
			txt->SetAnchorY(Anchor::Center);
			txt->SetAlignedSize(Vector2(1.0f, 0.75f));
			txt->GetWidgetProps().drawBackground  = true;
			txt->GetWidgetProps().colorBackground = Color::White;
			txt->GetWidgetProps().textureAtlas	  = img;
			txt->SetCustomTooltipUserData(txt);
			txt->SetBuildCustomTooltip(BIND(&CommonWidgets::BuildThumbnailTooltip, std::placeholders::_1));
			return txt;
		};

		for (const Row& row : rows)
		{
			widgets[0] = createText(row.displayName, Theme::GetDef().foreground0);
			widgets[1] = createText(TO_STRING(row.id), Theme::GetDef().foreground0);
			widgets[2] = createText(ReflectionSystem::Get().Resolve(row.type)->GetProperty<String>("DisplayName"_hs), ReflectionSystem::Get().Resolve(row.type)->GetProperty<Color>("Color"_hs));

			ResourceDirectory* dir = m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(row.id);

			if (dir == nullptr)
				widgets[3] = createText("-", Theme::GetDef().foreground0);
			else
				widgets[3] = createVis(m_editor->GetProjectManager().GetThumbnail(dir));

			widgets[4] = createText(row.size == 0 ? "-" : UtilStr::SizeBytesToString(row.size, 2), Theme::GetDef().foreground0);

			m_resourcesData.table->AddRow(widgets);
		}
	}

} // namespace Lina::Editor
