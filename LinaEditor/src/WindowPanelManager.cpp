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

#include "Editor/WindowPanelManager.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/Panel/Panel.hpp"
#include "Editor/Widgets/Panel/PanelResourceViewer.hpp"
#include "Editor/Widgets/Panel/PanelFactory.hpp"
#include "Editor/Widgets/Compound/ColorWheelCompound.hpp"
#include "Editor/Widgets/Panel/PanelColorWheel.hpp"
#include "Editor/Widgets/Panel/PanelGenericSelector.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Widgets/Popups/NotificationDisplayer.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"

#include "Common/Platform/LinaGXIncl.hpp"
#include "Core/Application.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void WindowPanelManager::Initialize(Editor* editor)
	{
		m_editor	 = editor;
		m_mainWindow = m_editor->GetApp()->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		CreateSurfaceRendererForWindow(m_editor->GetApp()->GetApplicationWindow(LINA_MAIN_SWAPCHAIN));

		m_payloadWindow = m_editor->CreateEditorWindow(PAYLOAD_WINDOW_SID, "Transparent", Vector2i(0, 0), Vector2(500, 500), (uint32)LinaGX::WindowStyle::BorderlessAlpha, m_mainWindow);
		CreateSurfaceRendererForWindow(m_payloadWindow);

		m_primaryWidgetManager = &GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();
		m_payloadWindow->SetVisible(false);
	}

	void WindowPanelManager::Shutdown()
	{
		for (auto* w : m_subWindows)
		{
			DestroySurfaceRenderer(w);
			m_editor->DestroyEditorWindow(w);
		}

		m_subWindows.clear();
		DestroySurfaceRenderer(m_payloadWindow);
		m_editor->DestroyEditorWindow(m_payloadWindow);
		DestroySurfaceRenderer(m_mainWindow);
	}

	void WindowPanelManager::CreateSurfaceRendererForWindow(LinaGX::Window* window)
	{
		SurfaceRenderer* renderer = new SurfaceRenderer(m_editor, window, Theme::GetDef().background0);
		m_editor->GetEditorRenderer().AddSurfaceRenderer(renderer);
		m_surfaceRenderers[window] = renderer;

		NotificationDisplayer* notificationDisplayer = renderer->GetWidgetManager().Allocate<NotificationDisplayer>();
		notificationDisplayer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_CONTROLS_DRAW_ORDER);
		notificationDisplayer->SetAlignedPos(Vector2::Zero);
		notificationDisplayer->SetAlignedSize(Vector2::One);
		notificationDisplayer->SetDrawOrder(FOREGROUND_DRAW_ORDER + 111);
		renderer->GetWidgetManager().GetRoot()->AddChild(notificationDisplayer);
	}

	NotificationDisplayer* WindowPanelManager::GetNotificationDisplayer(LinaGX::Window* window)
	{
		Vector<NotificationDisplayer*> disp;
		Widget::GetWidgetsOfType(disp, m_surfaceRenderers.at(window)->GetGUIRoot());
		return disp.at(0);
	}

	void WindowPanelManager::DestroySurfaceRenderer(LinaGX::Window* window)
	{
		auto windowIt = m_surfaceRenderers.find(window);
		LINA_ASSERT(windowIt != m_surfaceRenderers.end(), "");
		SurfaceRenderer* sr = windowIt->second;
		m_editor->GetEditorRenderer().RemoveSurfaceRenderer(sr);
		delete sr;
		m_surfaceRenderers.erase(windowIt);
	}

	void WindowPanelManager::PreTick()
	{
		if (!m_windowCloseRequests.empty())
		{
			if (!m_windowCloseRequests.empty())
				m_editor->GetApp()->JoinRender();

			for (auto sid : m_windowCloseRequests)
			{
				auto it = linatl::find_if(m_subWindows.begin(), m_subWindows.end(), [sid](LinaGX::Window* w) -> bool { return static_cast<StringID>(w->GetSID()) == sid; });

				if (it != m_subWindows.end())
				{
					if (m_payloadRequest.sourceWindow == *it)
						m_payloadRequest.sourceWindow = nullptr;

					m_subWindows.erase(it);
				}

				LinaGX::Window* window = m_editor->GetApp()->GetApplicationWindow(sid);
				DestroySurfaceRenderer(window);
				m_editor->DestroyEditorWindow(window);
				LinaGX::Window* top = Application::GetLGX()->GetWindowManager().GetTopWindow();
				if (top->GetSID() == PAYLOAD_WINDOW_SID)
				{
					Application::GetLGX()->GetWindowManager().PopWindowFromList(PAYLOAD_WINDOW_SID);
					top = Application::GetLGX()->GetWindowManager().GetTopWindow();
				}
				if (top != nullptr)
					top->BringToFront();
			}

			m_windowCloseRequests.clear();
		}

		if (m_payloadRequest.active)
		{
			if (!m_payloadWindow->GetIsVisible())
			{
				m_editor->GetApp()->JoinRender();

				m_payloadWindow->SetVisible(true);
				m_payloadWindow->SetAlpha(0.75f);
				m_payloadWindow->SetSize(m_payloadRequest.size.AsLGX2UI());

				m_payloadRequest.payload->GetFlags().Set(0);
				m_payloadRequest.payload->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
				m_payloadRequest.payload->SetAlignedPos(Vector2::Zero);
				m_payloadRequest.payload->SetAlignedSize(Vector2::One);

				GetPayloadRoot()->AddChild(m_payloadRequest.payload);
				GetPayloadRoot()->Initialize();

				for (auto* l : m_payloadListeners)
					l->OnPayloadStarted(m_payloadRequest.type, m_payloadRequest.payload);
			}

			const auto& mp = Application::GetLGX()->GetInput().GetMousePositionAbs();
			m_payloadWindow->SetPosition({static_cast<int32>(mp.x) + 10, static_cast<int32>(mp.y) + 10});

			if (!Application::GetLGX()->GetInput().GetMouseButton(LINAGX_MOUSE_0))
			{
				m_editor->GetApp()->JoinRender();

				bool received = false;
				for (auto* l : m_payloadListeners)
				{
					if (l->OnPayloadDropped(m_payloadRequest.type, m_payloadRequest.payload))
					{
						l->OnPayloadGetWindow()->BringToFront();
						received = true;
						break;
					}
				}

				for (auto* l : m_payloadListeners)
					l->OnPayloadEnded(m_payloadRequest.type, m_payloadRequest.payload);

				if (!received)
				{
					if (m_payloadRequest.sourceWindow)
						m_payloadRequest.sourceWindow->BringToFront();

					if (m_payloadRequest.type == PayloadType::DockedPanel)
					{
						PanelPayloadData* sub		= static_cast<PanelPayloadData*>(m_payloadRequest.payload->GetUserData());
						Widget*			  panelArea = PrepareNewWindowToDock(mp, sub->panelSize);
						Panel*			  panel		= PanelFactory::CreatePanel(panelArea, sub->type, sub->subData);
						FillPanelLayout(panel);
						panel->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
						panel->GetFlags().Remove(WF_POS_ALIGN_Y);
						panel->SetAlignedPosX(0.0f);
						panel->SetAlignedSize(Vector2(1.0f, 0.0f));
						panel->SetAlignedPos(Vector2::Zero);
						panel->Initialize();

						DockArea* dockArea = panelArea->GetWidgetManager()->Allocate<DockArea>("DockArea");
						dockArea->SetAlignedPos(Vector2::Zero);
						dockArea->SetAlignedSize(Vector2::One);
						panelArea->AddChild(dockArea);
						dockArea->AddPanel(panel);
						delete sub;
					}
				}

				m_payloadRequest.payload->GetParent()->RemoveChild(m_payloadRequest.payload);
				GetPayloadRoot()->GetWidgetManager()->Deallocate(m_payloadRequest.payload);
				m_payloadWindow->SetVisible(false);

				m_payloadRequest = {};
			}
		}
	}

	void WindowPanelManager::AddPayloadListener(EditorPayloadListener* listener)
	{
		m_payloadListeners.push_back(listener);
	}

	void WindowPanelManager::RemovePayloadListener(EditorPayloadListener* listener)
	{
		m_payloadListeners.erase(linatl::find(m_payloadListeners.begin(), m_payloadListeners.end(), listener));
	}

	void WindowPanelManager::CreatePayload(Widget* payload, PayloadType type, const Vector2ui& size)
	{
		m_payloadRequest.active		  = true;
		m_payloadRequest.payload	  = payload;
		m_payloadRequest.type		  = type;
		m_payloadRequest.size		  = size;
		m_payloadRequest.sourceWindow = payload->GetWindow();
	}

	Widget* WindowPanelManager::GetPayloadRoot()
	{
		return GetSurfaceRenderer(PAYLOAD_WINDOW_SID)->GetWidgetManager().GetRoot();
	}

	Panel* WindowPanelManager::FindPanelFromDockAreas(PanelType type, const Vector<DockArea*>& areas, DockArea*& outOwningArea, ResourceID subData)
	{
		Panel* retVal = nullptr;

		// Will try to return panel matching both type and subdata.
		// If can't find matching both but only type, it'll still return that.
		for (auto* area : areas)
		{
			const auto& panels = area->GetPanels();

			for (auto* panel : panels)
			{
				if (panel->GetType() == type)
				{
					outOwningArea = area;
					retVal		  = panel;
					if (subData == 0 || panel->GetSubData() == subData)
					{
						return panel;
					}
				}
			}
		}
		return retVal;
	}

	Panel* WindowPanelManager::FindPanelOfType(PanelType type, ResourceID subData)
	{
		DockArea* owning;
		Panel*	  panel = FindPanelOfType(type, subData, owning);
		return panel;
	}

	Vector<PanelResourceViewer*> WindowPanelManager::FindResourceViewers(ResourceID subData)
	{
		Vector<PanelResourceViewer*> panels;

		auto check = [&](PanelType pt) {
			Panel* panel = FindPanelOfType(pt, subData);
			if (panel != nullptr)
				panels.push_back(static_cast<PanelResourceViewer*>(panel));
		};

		check(PanelType::AudioViewer);
		check(PanelType::TextureViewer);
		check(PanelType::SamplerViewer);
		check(PanelType::FontViewer);
		check(PanelType::MaterialViewer);
		check(PanelType::ModelViewer);
		check(PanelType::PhysicsMaterialViewer);

		return panels;
	}

	void WindowPanelManager::FillPanelLayout(Panel* panel)
	{
		EditorLayout::PanelData pd = m_editor->GetSettings().GetLayout().FindPanelData(panel->GetType());

		if (!pd.layoutData.empty())
		{
			IStream stream;
			stream.Create(pd.layoutData);
			panel->LoadLayoutFromStream(stream);
			stream.Destroy();
		}
	}

	Panel* WindowPanelManager::FindPanelOfType(PanelType type, ResourceID subData, DockArea*& owningArea)
	{
		Vector<DockArea*> primaryAreas;
		Widget::GetWidgetsOfType<DockArea>(primaryAreas, m_primaryWidgetManager->GetRoot());
		Panel* foundPanel = FindPanelFromDockAreas(type, primaryAreas, owningArea, subData);
		if (foundPanel != nullptr && foundPanel->GetSubData() == subData)
			return foundPanel;

		for (auto* w : m_subWindows)
		{
			Vector<DockArea*> areas;
			SurfaceRenderer*  sf = GetSurfaceRenderer(static_cast<StringID>(w->GetSID()));
			Widget::GetWidgetsOfType<DockArea>(areas, sf->GetWidgetManager().GetRoot());

			foundPanel = FindPanelFromDockAreas(type, areas, owningArea, subData);

			if (foundPanel != nullptr && (subData == 0 || foundPanel->GetSubData() == subData))
				return foundPanel;
		}

		return foundPanel;
	}

	PanelColorWheel* WindowPanelManager::OpenColorWheelPanel(Widget* requester)
	{
		PanelColorWheel* wh	   = static_cast<PanelColorWheel*>(OpenPanel(PanelType::ColorWheel, 0, requester));
		m_panelColorWheel	   = wh;
		m_panelColorWheelOwner = requester;

		const Vector2 ms	 = m_mainWindow->GetMonitorSize();
		const float	  width	 = ms.x * 0.28f;
		const float	  height = width * 1.15f;
		wh->GetWindow()->SetSize(LinaGX::LGXVector2ui(width, height));

		const float posx = requester->GetWindow()->GetPosition().x + requester->GetPos().x / requester->GetWindow()->GetDPIScale();
		const float posy = requester->GetWindow()->GetPosition().y + (requester->GetPos().y + requester->GetSize().y) / requester->GetWindow()->GetDPIScale() + Theme::GetDef().baseIndent;
		wh->GetWindow()->SetPosition({static_cast<int32>(posx), static_cast<int32>(posy)});

		return wh;
	}

	PanelGenericSelector* WindowPanelManager::OpenGenericSelectorPanel(Widget* requester)
	{
		PanelGenericSelector* panel = static_cast<PanelGenericSelector*>(OpenPanel(PanelType::GenericSelector, 0, requester));

		const float width = panel->GetWindow()->GetMonitorSize().x * 0.25f;
		panel->GetWindow()->SetSize({static_cast<uint32>(width), static_cast<uint32>(width * 1.15f)});

		const float posx = requester->GetWindow()->GetPosition().x + requester->GetPos().x / requester->GetWindow()->GetDPIScale();
		const float posy = requester->GetWindow()->GetPosition().y + (requester->GetPos().y + requester->GetSize().y) / requester->GetWindow()->GetDPIScale() + Theme::GetDef().baseIndent;
		panel->GetWindow()->SetPosition({static_cast<int32>(posx), static_cast<int32>(posy)});
		return panel;
	}

	Panel* WindowPanelManager::OpenPanel(PanelType type, ResourceID subData, Widget* requestingWidget)
	{
		DockArea* owningArea = nullptr;
		Panel*	  foundPanel = FindPanelOfType(type, subData, owningArea);

		if (foundPanel != nullptr && foundPanel->GetSubData() == subData)
		{
			foundPanel->GetWindow()->BringToFront();
			owningArea->SetSelected(foundPanel);
			return foundPanel;
		}

		// We haven't found the exact same panel, but we found a panel of same type with different subdata.
		if (foundPanel != nullptr)
		{
			Panel* panel = PanelFactory::CreatePanel(owningArea, type, subData);
			FillPanelLayout(panel);
			owningArea->AddPanel(panel);
			return panel;
		}

		// Not found, create a window & insert panel.
		Vector2 pos		  = Vector2::Zero;
		Vector2 panelSize = Vector2(500, 500);

		if (requestingWidget)
		{
			pos				   = requestingWidget->GetWindow()->GetPosition();
			const Vector2ui sz = requestingWidget->GetWindow()->GetMonitorInfoFromWindow().size;
			panelSize		   = Vector2(static_cast<float>(sz.x) * 0.4f, static_cast<float>(sz.y) * 0.4f);
		}
		else
		{
			const Vector2ui sz = m_mainWindow->GetMonitorInfoFromWindow().size;
			panelSize		   = Vector2(static_cast<float>(sz.x) * 0.4f, static_cast<float>(sz.y) * 0.4f);
			pos				   = m_mainWindow->GetPosition();
		}

		Widget* panelArea = PrepareNewWindowToDock(pos, panelSize);
		m_subWindowCounter++;

		DockArea* dock = panelArea->GetWidgetManager()->Allocate<DockArea>("DockArea");
		dock->SetAlignedPos(Vector2::Zero);
		dock->SetAlignedSize(Vector2::One);
		panelArea->AddChild(dock);

		Panel* panel = PanelFactory::CreatePanel(dock, type, subData);
		dock->GetWindow()->SetTitle(panel->GetWidgetProps().debugName);
		dock->AddPanel(panel);

		FillPanelLayout(panel);

		auto info = m_windowPanelInfos.find(type);
		if (info != m_windowPanelInfos.end())
		{
			WindowPanelInfo& inf = info->second;
			dock->GetWindow()->SetPosition(inf.position.AsLGX2I());
			dock->GetWindow()->SetSize(inf.size.AsLGX2UI());
		}

		return panel;
	}

	Widget* WindowPanelManager::PrepareNewWindowToDock(const Vector2& pos, const Vector2& size)
	{
		const String	title	 = "Lina Editor " + TO_STRING(m_subWindowCounter);
		const Vector2	usedSize = size.Clamp(m_editor->GetEditorRoot()->GetMonitorSize() * 0.1f, m_editor->GetEditorRoot()->GetMonitorSize());
		LinaGX::Window* window	 = m_editor->CreateEditorWindow(m_subWindowCounter, title.c_str(), pos, usedSize, (uint32)LinaGX::WindowStyle::BorderlessApplication, m_mainWindow);
		CreateSurfaceRendererForWindow(window);

		m_subWindows.push_back(window);
		Widget*			   newWindowRoot = GetSurfaceRenderer(m_subWindowCounter)->GetWidgetManager().GetRoot();
		DirectionalLayout* layout		 = newWindowRoot->GetWidgetManager()->Allocate<DirectionalLayout>("BaseLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		newWindowRoot->AddChild(layout);

		WindowBar* wb						 = newWindowRoot->GetWidgetManager()->Allocate<WindowBar>("WindowBar");
		wb->GetBarProps().title				 = "Lina Engine";
		wb->GetBarProps().icon				 = ICON_LINA_LOGO;
		wb->GetBarProps().hasWindowButtons	 = true;
		wb->GetBarProps().controlsDragRect	 = true;
		wb->GetWidgetProps().drawBackground	 = true;
		wb->GetWidgetProps().colorBackground = Theme::GetDef().accentPrimary0;
		wb->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		wb->SetAlignedPosX(0.0f);
		wb->SetAlignedSizeX(1.0f);
		wb->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		layout->AddChild(wb);
		layout->Initialize();

		Widget* panelArea = newWindowRoot->GetWidgetManager()->Allocate<Widget>("PanelArea");
		panelArea->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_ALIGN_Y);
		panelArea->SetAlignedPosX(0.0f);
		panelArea->SetAlignedSize(Vector2(1.0f, 0.0f));
		layout->AddChild(panelArea);

		m_subWindowCounter++;
		return panelArea;
	}

	void WindowPanelManager::CloseAllSubwindows()
	{
		for (auto* w : m_subWindows)
			CloseWindow(static_cast<StringID>(w->GetSID()));
	}

	void WindowPanelManager::CloseWindow(StringID sid)
	{
		m_windowCloseRequests.push_back(sid);
	}

	void WindowPanelManager::OnPanelDestruct(Panel* panel)
	{
		if (panel->GetType() == PanelType::ColorWheel)
		{

			m_panelColorWheel	   = nullptr;
			m_panelColorWheelOwner = nullptr;
		}

		WindowPanelInfo& inf = m_windowPanelInfos[panel->GetType()];
		inf.position		 = panel->GetWindow()->GetPosition();
		inf.size			 = panel->GetWindow()->GetSize();
		inf.subdata			 = panel->GetSubData();
	}

	SurfaceRenderer* WindowPanelManager::GetSurfaceRenderer(StringID sid)
	{
		return m_surfaceRenderers.at(m_editor->GetApp()->GetApplicationWindow(sid));
	}

	Widget* WindowPanelManager::LockAllForegrounds(LinaGX::Window* srcWindow, Delegate<Widget*(Widget* owner)> otherWindowContents)
	{
		Widget* lock = nullptr;

		for (Pair<LinaGX::Window*, SurfaceRenderer*> pair : m_surfaceRenderers)
		{
			if (pair.first == srcWindow)
			{
				lock	   = pair.second->GetWidgetManager().LockForeground();
				m_mainLock = lock;
				continue;
			}

			Widget* otherLock = pair.second->GetWidgetManager().LockForeground();

			if (otherWindowContents != nullptr)
			{
				Widget* w = otherWindowContents(otherLock);
				otherLock->AddChild(w);
			}
		}

		return lock;
	}

	void WindowPanelManager::UnlockAllForegrounds()
	{
		for (Pair<LinaGX::Window*, SurfaceRenderer*> pair : m_surfaceRenderers)
		{
			pair.second->GetWidgetManager().UnlockForeground();
		}
		m_mainLock = nullptr;
	}
} // namespace Lina::Editor
