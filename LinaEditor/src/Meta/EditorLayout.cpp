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

#include "Editor/Meta/EditorLayout.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockBorder.hpp"
#include "Editor/Widgets/Panel/Panel.hpp"
#include "Editor/Widgets/Panel/PanelFactory.hpp"
#include "Common/System/System.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"

namespace Lina::Editor
{
	void EditorLayout::LoadFromStream(IStream& in)
	{
		uint32 version = 0;
		in >> version;
		VectorSerialization::LoadFromStream_OBJ(in, m_windows);
	}

	void EditorLayout::SaveToStream(OStream& out)
	{
		out << VERSION;
		VectorSerialization::SaveToStream_OBJ(out, m_windows);
	}

	void EditorLayout::StoreDefaultLayout()
	{
		m_windows.clear();

		WindowLayout wl = {};
		wl.sid			= LINA_MAIN_SWAPCHAIN;

		// Entities
		wl.dockWidgets.push_back({
			.alignedPos	 = Vector2::Zero,
			.alignedSize = Vector2(0.25f, 0.75f),
			.panels		 = {{.panelType = PanelType::Entities}},
		});

		// World
		wl.dockWidgets.push_back({
			.alignedPos	 = Vector2(0.25f, 0.0f),
			.alignedSize = Vector2(0.75f, 0.75f),
			.panels		 = {{.panelType = PanelType::World}},
		});

		// Resources
		wl.dockWidgets.push_back({
			.alignedPos	 = Vector2(0.0f, 0.75f),
			.alignedSize = Vector2(1.0f, 0.25f),
			.panels		 = {{.panelType = PanelType::Resources, .extraData = {.data0 = 0.25f}}},
		});

		// Border: Entities | World
		wl.dockWidgets.push_back({
			.alignedPos	  = Vector2(0.25f, 0.0f),
			.alignedSize  = Vector2(0.0f, 0.75f),
			.isBorder	  = true,
			.isHorizontal = false,
		});

		// Border: Upper | Resources
		wl.dockWidgets.push_back({
			.alignedPos	  = Vector2(0.0f, 0.75f),
			.alignedSize  = Vector2(1.0f, 0.0f),
			.isBorder	  = true,
			.isHorizontal = true,
		});

		m_windows.push_back(wl);
	}

	void EditorLayout::ApplyStoredLayout(Editor* editor)
	{
		if (m_windows.empty())
			StoreDefaultLayout();

		GfxManager* gfxMan = editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		for (const auto& windowData : m_windows)
		{
			Widget* panelArea = nullptr;
			if (windowData.sid == LINA_MAIN_SWAPCHAIN)
			{
				panelArea = editor->GetEditorRoot()->GetPanelArea();

				if (windowData.size.x != 0 && windowData.size.y != 0)
				{
					gfxMan->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->SetPosition(windowData.position.AsLGX2I());
					gfxMan->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->AddSizeRequest(windowData.size.AsLGX2UI());
				}
			}
			else
				panelArea = editor->PrepareNewWindowToDock(windowData.sid, windowData.position, windowData.size, windowData.title);

			for (const auto& dockWidget : windowData.dockWidgets)
			{
				if (dockWidget.isBorder)
				{
					DockBorder* border = panelArea->GetWidgetManager()->Allocate<DockBorder>("Border");
					border->SetAlignedPos(dockWidget.alignedPos);
					border->SetAlignedSize(dockWidget.alignedSize);
					border->SetDirectionOrientation(dockWidget.isHorizontal ? DirectionOrientation::Horizontal : DirectionOrientation::Vertical);
					border->Initialize();
					panelArea->AddChild(border);
				}
				else
				{
					DockArea* area = panelArea->GetWidgetManager()->Allocate<DockArea>("DockArea");
					area->SetAlignedPos(dockWidget.alignedPos);
					area->SetAlignedSize(dockWidget.alignedSize);
					area->Initialize();
					panelArea->AddChild(area);

					for (const auto& panelData : dockWidget.panels)
					{
						Panel* panel = PanelFactory::CreatePanel(panelArea, panelData.panelType, panelData.subData);
						area->AddPanel(panel);
						panel->SetExtraLayoutData(panelData.extraData);
					}
				}
			}

			for (size_t i = 0; i < windowData.dockWidgets.size(); i++)
			{
				if (windowData.dockWidgets[i].isBorder)
					static_cast<DockBorder*>(panelArea->GetChildren()[i])->FixChildMargins();
			}
		}
	}

	void EditorLayout::StoreLayout(Editor* editor)
	{
		m_windows.clear();

		GfxManager* gfxMan = editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		Vector<LinaGX::Window*> windows = editor->GetSubWindows();
		windows.push_back(gfxMan->GetApplicationWindow(LINA_MAIN_SWAPCHAIN));

		for (auto* w : windows)
		{
			Widget* windowRoot = gfxMan->GetSurfaceRenderer(static_cast<StringID>(w->GetSID()))->GetWidgetManager().GetRoot();

			Vector<DockArea*> dockAreas;
			Widget::GetWidgetsOfType<DockArea>(dockAreas, windowRoot);

			if (!dockAreas.empty())
			{
				WindowLayout wl = {};
				wl.sid			= static_cast<StringID>(w->GetSID());
				wl.position		= w->GetPosition();
				wl.size			= w->GetSize();
				wl.title		= w->GetTitle();

				for (auto* area : dockAreas)
				{
					DockWidgetData dwd = {};
					dwd.alignedPos	   = area->GetAlignedPos();
					dwd.alignedSize	   = area->GetAlignedSize();
					dwd.isBorder	   = false;

					for (auto* panel : area->GetPanels())
					{
						PanelData pd = {};
						pd.panelType = panel->GetType();
						pd.subData	 = panel->GetSubData();
						pd.extraData = panel->GetExtraLayoutData();
						dwd.panels.push_back(pd);
					}

					wl.dockWidgets.push_back(dwd);
				}

				Vector<DockBorder*> borders;
				Widget::GetWidgetsOfType<DockBorder>(borders, windowRoot);

				for (auto* border : borders)
				{
					DockWidgetData dwd = {};
					dwd.alignedPos	   = border->GetAlignedPos();
					dwd.alignedSize	   = border->GetAlignedSize();
					dwd.isBorder	   = true;
					dwd.isHorizontal   = border->GetDirectionOrientation() == DirectionOrientation::Horizontal;
					wl.dockWidgets.push_back(dwd);
				}

				m_windows.push_back(wl);
			}
		}
	}
} // namespace Lina::Editor
