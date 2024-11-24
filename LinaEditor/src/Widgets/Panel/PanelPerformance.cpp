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
#include "Editor/EditorLocale.hpp"
#include "Editor/Editor.hpp"
#include "Core/Application.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina::Editor
{
#define RECORD_LAST_N_FRAMES 500

	void PanelPerformance::Construct()
	{
		m_editor = Editor::Get();

		const uint8 selectedTab	  = m_editor->GetSettings().GetParams().GetParamUint8("panelPerformanceTab"_hs, 0);
		const uint8 resourcesSort = m_editor->GetSettings().GetParams().GetParamUint8("panelPerformanceResSort"_hs, 0);

		m_resourcesSort = static_cast<ResourcesSort>(resourcesSort);

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
				.icon	 = ICON_MEMORY,
				.tooltip = Locale::GetStr(LocaleStr::Memory),
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

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("ANAN");
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

	void PanelPerformance::Tick(float delta)
	{
		if (m_currentContent == 0)
		{
			LineGraph::PointGroup& group = m_graphFrameTime->GetProps().groups.back();
			// group.points.resize(RECORD);

			m_profilingLastFrames.push_back(delta);

			if (m_profilingLastFrames.size() > RECORD_LAST_N_FRAMES)
				m_profilingLastFrames.pop_front();

			static float lastTime = 0.0f;

			lastTime += delta;

			if (lastTime > 0.01f && m_profilingLastFrames.size() == 500)
			{
				lastTime   = 0.0f;
				uint32 idx = 0;

				for (float f : m_profilingLastFrames)
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

		if (m_resourcesTable != nullptr)
		{
			const Vector<Widget*> children = m_resourcesTable->GetHorizontalLayout()->GetChildren();
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
		uint32 sz = 0;
		stream >> sz;

		for (uint32 i = 0; i < sz; i++)
		{
			float posX = 0.0f, sizeX = 0.0f;
			stream >> posX >> sizeX;
			if (m_resourcesTable != nullptr)
			{
				const Vector<Widget*>& children = m_resourcesTable->GetHorizontalLayout()->GetChildren();
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

		m_resourcesTable = nullptr;
		m_graphFrameTime = nullptr;

		m_currentContent = idx;

		if (idx == 0)
			BuildContentsProfiling();
		else if (idx == 1)
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

		frameTime->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::FrameTime) + ": - ms");
		frameTime->GetProps().isDynamic = true;
		frameTime->SetTickHook([frameTime](float delta) {
			static float elapsed = 1.0f;

			elapsed += delta;

			if (elapsed > 1.0f)
			{
				const float	 fps = 1.0f / delta;
				const String str = UtilStr::FloatToString(delta * 1000.0f, 8) + " ms - " + TO_STRING(static_cast<int32>(fps)) + " (FPS)";
				frameTime->UpdateTextAndCalcSize(Locale::GetStr(LocaleStr::FrameTime) + ": " + str);
				elapsed = 0.0f;
			}
		});
		m_layout->AddChild(wrap(frameTime));

		// Table* table = m_manager->Allocate<Table>("Table");
		// table->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		// table->SetAlignedPosX(0.0f);
		// table->SetAlignedSize(Vector2(1.0f, 0.0f));
		// m_layout->AddChild(table);

		m_graphFrameTime = m_manager->Allocate<LineGraph>("Graph");
		m_graphFrameTime->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		m_graphFrameTime->SetAlignedPosX(0.0f);
		m_graphFrameTime->SetAlignedSizeX(1.0f);
		m_graphFrameTime->SetFixedSizeY(Theme::GetDef().baseItemHeight * 15);

		m_graphFrameTime->GetWidgetProps().drawBackground  = true;
		m_graphFrameTime->GetWidgetProps().colorBackground = Theme::GetDef().background1;

		m_graphFrameTime->GetProps().drawBackground	   = true;
		m_graphFrameTime->GetProps().colorBackground   = Theme::GetDef().background0;
		m_graphFrameTime->GetProps().colorBarMainAxis  = Theme::GetDef().silent1;
		m_graphFrameTime->GetProps().colorBarCrossAxis = Theme::GetDef().silent1;

		m_graphFrameTime->GetProps().gridCells.y				  = 5;
		m_graphFrameTime->GetProps().colorGridHorizontal		  = Theme::GetDef().silent0;
		m_graphFrameTime->GetProps().crossAxisDynamic			  = true;
		m_graphFrameTime->GetProps().colorTextMainAxisInterpolate = true;
		m_graphFrameTime->GetProps().colorTextMainAxis			  = ColorGrad(Theme::GetDef().accentPrimary0, Theme::GetDef().accentSecondary);
		m_graphFrameTime->GetProps().title						  = "FPS/Frames";

		m_graphFrameTime->GetProps().legends = {
			{
				.axis  = "X:",
				.text  = "Last Frames",
				.color = Theme::GetDef().accentSecondary,
			},
			{
				.axis  = "Y:",
				.text  = "FPS",
				.color = Theme::GetDef().accentSecondary,
			},
		};

		m_graphFrameTime->GetProps().groups = {
			{
				.colorLine = ColorGrad(Theme::GetDef().accentPrimary0, Theme::GetDef().accentSecondary),
				.drawLine  = true,
			},
		};

		m_graphFrameTime->GetProps().groups.back().points.resize(RECORD_LAST_N_FRAMES);

		m_graphFrameTime->GetProps().crossAxisPoints = {
			{
				.value		 = 680,
				.displayName = "680",
			},
			{
				.value		 = 620.0f,
				.displayName = "620",
			},
			{
				.value		 = 120.0f,
				.displayName = "120",
			},
			{
				.value		 = 60.0f,
				.displayName = "60",
			},
			{
				.value		 = 30.0f,
				.displayName = "30",
			},
			{
				.value		 = 15.0f,
				.displayName = "15",
			},
		};

		m_graphFrameTime->GetProps().mainAxisPoints = {
			{
				.value		 = 0.0f,
				.displayName = "",
			},
			{
				.value		 = 25.0f,
				.displayName = "Frame 25",
			},
			{
				.value		 = 50.0f,
				.displayName = "Frame 50",
			},
			{
				.value		 = 75.0f,
				.displayName = "Frame 75",
			},
			{
				.value		 = 500.0f,
				.displayName = "Frame 500",
			},
		};

		m_layout->AddChild(m_graphFrameTime);
	}

	void PanelPerformance::BuildContentsMemory()
	{
	}

	void PanelPerformance::BuildContentsResources()
	{
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

		// Dropdown* showDD = m_manager->Allocate<Dropdown>("DD");
		// showDD->GetFlags().Set(WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_SIZE_ALIGN_Y);
		// showDD->SetAlignedPosY(0.0f);
		// showDD->SetAlignedSizeY(1.0f);
		// showDD->SetFixedSize(Theme::GetDef().baseItemWidth);
		// header->AddChild(showDD);

		InputField* search = m_manager->Allocate<InputField>("Search");
		search->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		search->SetAlignedPosY(0.0f);
		search->SetAlignedSize(Vector2(0.0f, 1.0f));
		search->GetProps().placeHolderIcon = ICON_SEARCH;
		search->GetProps().placeHolderText = Locale::GetStr(LocaleStr::Search);
		search->GetProps().onEdited		   = [this](const String& str) {
			   m_resourcesSearchStr = UtilStr::ToLower(str);
			   RefreshResourcesTable();
		};

		header->AddChild(search);

		m_resourcesTable													 = m_manager->Allocate<Table>("Table");
		m_resourcesTable->GetHorizontalLayout()->GetProps().onBordersChanged = [this]() { m_editor->SaveSettings(); };
		m_resourcesTable->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_resourcesTable->SetAlignedPosX(0.0f);
		m_resourcesTable->SetAlignedSize(Vector2(1.0f, 0.0f));
		m_resourcesTable->GetHorizontalLayout()->GetProps().borderMinSize = 0.08f;
		m_resourcesTable->BuildHeaders({
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

		m_resourcesTable->GetProps().onColumnClicked = [this](uint32 idx) {
			const ResourcesSort prev = m_resourcesSort;

			if (idx == 0)
				m_resourcesSort = ResourcesSort::Name;
			if (idx == 1)
				m_resourcesSort = ResourcesSort::ID;
			if (idx == 2)
				m_resourcesSort = ResourcesSort::Type;
			if (idx == 4)
				m_resourcesSort = ResourcesSort::Size;

			if (prev != m_resourcesSort)
			{
				m_editor->GetSettings().GetParams().SetParamUint8("panelPerformanceResSort"_hs, static_cast<uint8>(m_resourcesSort));
				m_editor->SaveSettings();
				RefreshResourcesTable();
			}
		};
		m_layout->AddChild(m_resourcesTable);
		RefreshResourcesTable();
	}

	void PanelPerformance::RefreshResourcesTable()
	{
		m_resourcesTable->ClearRows();

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

				if (!m_resourcesSearchStr.empty() && UtilStr::ToLower(name).find(m_resourcesSearchStr) == String::npos)
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
			if (m_resourcesSort == ResourcesSort::Name)
				return r1.displayName < r2.displayName;

			if (m_resourcesSort == ResourcesSort::ID)
				return r1.id < r2.id;

			if (m_resourcesSort == ResourcesSort::Type)
				return r1.type < r2.type;

			if (m_resourcesSort == ResourcesSort::Size)
				return r1.size < r2.size;

			return false;
		});

		Vector<Widget*> widgets;
		widgets.resize(5);

		auto createText = [&](const String& title, const Color& background, const Color& textColor) -> Widget* {
			Widget* wrap = m_manager->Allocate<Widget>("Wrap");
			wrap->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
			wrap->SetAlignedPosX(0.0f);
			wrap->SetAlignedSizeX(1.0f);
			wrap->SetFixedSizeY(Theme::GetDef().baseItemHeight);
			wrap->GetWidgetProps().childMargins.left = Theme::GetDef().baseIndent;
			wrap->GetWidgetProps().drawBackground	 = true;
			wrap->GetWidgetProps().outlineThickness	 = 0.0f;
			wrap->GetWidgetProps().rounding			 = 0.0f;
			wrap->GetWidgetProps().colorBackground	 = background;

			Text* txt = m_manager->Allocate<Text>("Txt");
			txt->UpdateTextAndCalcSize(title);
			txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			txt->SetAlignedPos(Vector2(0.0f, 0.5f));
			txt->SetAnchorY(Anchor::Center);
			txt->GetProps().color = textColor;
			wrap->AddChild(txt);

			return wrap;
		};

		auto createVis = [&](TextureAtlasImage* img, const Color& background) -> Widget* {
			Widget* wrap = m_manager->Allocate<Widget>("Wrap");
			wrap->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
			wrap->SetAlignedPosX(0.0f);
			wrap->SetAlignedSizeX(1.0f);
			wrap->SetFixedSizeY(Theme::GetDef().baseItemHeight);
			wrap->GetWidgetProps().childMargins.left = Theme::GetDef().baseIndent;
			wrap->GetWidgetProps().drawBackground	 = true;
			wrap->GetWidgetProps().outlineThickness	 = 0.0f;
			wrap->GetWidgetProps().rounding			 = 0.0f;
			wrap->GetWidgetProps().colorBackground	 = background;

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
			wrap->AddChild(txt);
			return wrap;
		};

		bool colorSwitch = false;

		for (const Row& row : rows)
		{
			const Color color = colorSwitch ? Theme::GetDef().background2 : Theme::GetDef().background1;
			colorSwitch		  = !colorSwitch;

			widgets[0] = createText(row.displayName, color, Theme::GetDef().foreground0);
			widgets[1] = createText(TO_STRING(row.id), color, Theme::GetDef().foreground0);
			widgets[2] = createText(ReflectionSystem::Get().Resolve(row.type)->GetProperty<String>("DisplayName"_hs), color, ReflectionSystem::Get().Resolve(row.type)->GetProperty<Color>("Color"_hs));

			ResourceDirectory* dir = m_editor->GetProjectManager().GetProjectData()->GetResourceRoot().FindResourceDirectory(row.id);

			if (dir == nullptr)
				widgets[3] = createText("-", color, Theme::GetDef().foreground0);
			else
				widgets[3] = createVis(m_editor->GetProjectManager().GetThumbnail(dir), color);

			widgets[4] = createText(row.size == 0 ? "-" : UtilStr::SizeBytesToString(row.size, 2), color, Theme::GetDef().foreground0);

			m_resourcesTable->AddRow(widgets);
		}
	}

} // namespace Lina::Editor
