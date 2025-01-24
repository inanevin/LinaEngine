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
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"

#include "Core/Application.hpp"

namespace Lina::Editor
{
	EditorLayout::~EditorLayout()
	{
		ClearBuffers();
	}

	void EditorLayout::ClearBuffers()
	{
		for (WindowLayout& wl : m_windows)
		{
			for (DockWidgetData& dwd : wl.dockWidgets)
			{
				for (PanelData& pd : dwd.panels)
				{
					if (pd.layoutData.data() != nullptr)
						delete[] pd.layoutData.data();
					pd.layoutData = {};
				}
			}
		}
		m_windows.clear();
	}

	void EditorLayout::LoadFromStream(IStream& stream)
	{
		stream >> m_windows;
	}

	void EditorLayout::SaveToStream(OStream& stream)
	{
		stream << m_windows;
	}

	void EditorLayout::StoreDefaultLayout()
	{
		ClearBuffers();

		WindowLayout wl = {};
		wl.sid			= LINA_MAIN_SWAPCHAIN;

		// Entities
		wl.dockWidgets.push_back({
			.alignedPos	 = Vector2::Zero,
			.alignedSize = Vector2(0.2f, 0.3f),
			.panels		 = {{.panelType = PanelType::Entities}},
		});

		// Details
		wl.dockWidgets.push_back({
			.alignedPos	 = Vector2(0.0f, 0.3f),
			.alignedSize = Vector2(0.2f, 0.3f),
			.panels		 = {{.panelType = PanelType::Details}},
		});

		// Resource directory browser
		wl.dockWidgets.push_back({
			.alignedPos	 = Vector2(0.0f, 0.6f),
			.alignedSize = Vector2(0.2f, 0.4f),
			.panels		 = {{.panelType = PanelType::ResourceBrowser}},
		});

		// World
		wl.dockWidgets.push_back({
			.alignedPos	 = Vector2(0.2f, 0.0f),
			.alignedSize = Vector2(0.8f, 0.8f),
			.panels		 = {{.panelType = PanelType::World}},
		});

		// Log
		wl.dockWidgets.push_back({
			.alignedPos	 = Vector2(0.2f, 0.8f),
			.alignedSize = Vector2(0.8f, 0.2f),
			.panels		 = {{.panelType = PanelType::Log},
							{
								.panelType = PanelType::Performance,
						}},
		});

		// Border: Entities | Details
		wl.dockWidgets.push_back({
			.alignedPos	  = Vector2(0.0, 0.3f),
			.alignedSize  = Vector2(0.2f, 0.0f),
			.isBorder	  = true,
			.isHorizontal = true,
		});

		// Border: Details | Browser
		wl.dockWidgets.push_back({
			.alignedPos	  = Vector2(0.0, 0.6f),
			.alignedSize  = Vector2(0.2f, 0.0f),
			.isBorder	  = true,
			.isHorizontal = true,
		});

		// Border: World | Log
		wl.dockWidgets.push_back({
			.alignedPos	  = Vector2(0.2f, 0.8f),
			.alignedSize  = Vector2(0.8f, 0.0f),
			.isBorder	  = true,
			.isHorizontal = true,
		});

		// Border: Left | Right
		wl.dockWidgets.push_back({
			.alignedPos	  = Vector2(0.2f, 0.0f),
			.alignedSize  = Vector2(0.0f, 1.0f),
			.isBorder	  = true,
			.isHorizontal = false,
		});

		m_windows.push_back(wl);
	}

	EditorLayout::PanelData EditorLayout::FindPanelData(PanelType type)
	{
		for (const WindowLayout& wl : m_windows)
		{
			for (const DockWidgetData& dwd : wl.dockWidgets)
			{
				int32 i = 0;
				for (const PanelData& pd : dwd.panels)
				{
					if (pd.panelType == type)
						return pd;
				}
			}
		}

		return {};
	}

	void EditorLayout::ApplyStoredLayout()
	{
		if (m_windows.empty())
			StoreDefaultLayout();

		Editor* editor = Editor::Get();

		for (const WindowLayout& windowData : m_windows)
		{
			Widget* panelArea = nullptr;
			if (windowData.sid == LINA_MAIN_SWAPCHAIN)
			{
				panelArea = editor->GetEditorRoot()->GetPanelArea();

				if (windowData.size.x != 0 && windowData.size.y != 0)
				{
					editor->GetApp()->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->SetPosition(windowData.position.AsLGX2I());
					editor->GetApp()->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->AddSizeRequest(windowData.size.AsLGX2UI());
				}
			}
			else
				panelArea = editor->GetWindowPanelManager().PrepareNewWindowToDock(windowData.position, windowData.size);

			for (const DockWidgetData& dockWidget : windowData.dockWidgets)
			{
				if (dockWidget.isBorder)
				{
					DockBorder* border						  = panelArea->GetWidgetManager()->Allocate<DockBorder>("Border");
					border->GetWidgetProps().drawBackground	  = true;
					border->GetWidgetProps().outlineThickness = border->GetWidgetProps().rounding = 0.0f;
					border->GetWidgetProps().colorBackground									  = Theme::GetDef().background0;
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

					for (const PanelData& panelData : dockWidget.panels)
					{
						Panel* panel = PanelFactory::CreatePanel(panelArea, panelData.panelType, panelData.subData);
						area->AddPanel(panel);

						if (!panelData.layoutData.empty())
						{
							IStream stream;
							stream.Create(panelData.layoutData);
							panel->LoadLayoutFromStream(stream);
							stream.Destroy();
						}
					}

					area->SetSelected(area->GetPanels().at(dockWidget.selectedPanel));
				}
			}

			for (size_t i = 0; i < windowData.dockWidgets.size(); i++)
			{
				if (windowData.dockWidgets[i].isBorder)
					static_cast<DockBorder*>(panelArea->GetChildren()[i])->FixChildMargins();
			}
		}
	}

	void EditorLayout::StoreLayout()
	{
		ClearBuffers();

		Editor* editor = Editor::Get();

		Vector<LinaGX::Window*> windows = editor->GetWindowPanelManager().GetSubWindows();
		windows.push_back(editor->GetApp()->GetApplicationWindow(LINA_MAIN_SWAPCHAIN));

		for (auto* w : windows)
		{
			Widget* windowRoot = editor->GetWindowPanelManager().GetSurfaceRenderer(static_cast<StringID>(w->GetSID()))->GetWidgetManager().GetRoot();

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

					uint32 idx = 0;
					for (auto* panel : area->GetPanels())
					{
						PanelData pd = {};
						pd.panelType = panel->GetType();
						pd.subData	 = panel->GetSubData();
						OStream out;
						panel->SaveLayoutToStream(out);
						out.WriteTo(pd.layoutData);
						out.Destroy();
						dwd.panels.push_back(pd);

						if (panel == area->GetSelectedPanel())
							dwd.selectedPanel = idx;

						idx++;
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
