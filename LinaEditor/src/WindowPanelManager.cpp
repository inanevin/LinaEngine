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
#include "Editor/Widgets/Panel/PanelFactory.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Widgets/Popups/NotificationDisplayer.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"
#include "Common/System/System.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void WindowPanelManager::Initialize(Editor* editor)
	{
		m_editor		= editor;
		m_gfxManager	= m_editor->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_mainWindow	= m_gfxManager->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		m_payloadWindow = m_gfxManager->CreateApplicationWindow(PAYLOAD_WINDOW_SID, "Transparent", Vector2i(0, 0), Vector2(500, 500), (uint32)LinaGX::WindowStyle::BorderlessAlpha, m_mainWindow);

		CreateSurfaceRendererForWindow(m_gfxManager->GetApplicationWindow(LINA_MAIN_SWAPCHAIN));
		m_primaryWidgetManager = &GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		CreateSurfaceRendererForWindow(m_payloadWindow);
		m_payloadWindow->SetVisible(false);
	}

	void WindowPanelManager::Shutdown()
	{
		for (auto* w : m_subWindows)
		{
			DestroySurfaceRenderer(w);
			m_gfxManager->DestroyApplicationWindow(static_cast<StringID>(w->GetSID()));
		}

		m_subWindows.clear();
		DestroySurfaceRenderer(m_payloadWindow);
		m_gfxManager->DestroyApplicationWindow(static_cast<StringID>(m_payloadWindow->GetSID()));
		DestroySurfaceRenderer(m_mainWindow);
	}

	void WindowPanelManager::CreateSurfaceRendererForWindow(LinaGX::Window* window)
	{
		SurfaceRenderer* renderer = new SurfaceRenderer(m_editor, window, Theme::GetDef().background0);
		m_editor->GetEditorRenderer().AddSurfaceRenderer(renderer);
		m_surfaceRenderers[window] = renderer;

		NotificationDisplayer* notificationDisplayer = renderer->GetWidgetManager().Allocate<NotificationDisplayer>();
		notificationDisplayer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		notificationDisplayer->SetAlignedPos(Vector2::Zero);
		notificationDisplayer->SetAlignedSize(Vector2::One);
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
				m_gfxManager->Join();

			for (auto sid : m_windowCloseRequests)
			{
				auto it = linatl::find_if(m_subWindows.begin(), m_subWindows.end(), [sid](LinaGX::Window* w) -> bool { return static_cast<StringID>(w->GetSID()) == sid; });

				if (it != m_subWindows.end())
				{
					if (m_payloadRequest.sourceWindow == *it)
						m_payloadRequest.sourceWindow = nullptr;

					m_subWindows.erase(it);
				}
				LinaGX::Window* window = m_gfxManager->GetApplicationWindow(sid);
				DestroySurfaceRenderer(window);
				m_gfxManager->DestroyApplicationWindow(sid);
			}

			m_windowCloseRequests.clear();
		}

		if (m_payloadRequest.active)
		{
			if (!m_payloadWindow->GetIsVisible())
			{
				m_gfxManager->Join();

				m_payloadWindow->SetVisible(true);
				m_payloadWindow->SetAlpha(0.5f);
				m_payloadWindow->SetSize(m_payloadRequest.size.AsLGX2UI());

				m_payloadRequest.payload->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
				m_payloadRequest.payload->SetAlignedPos(Vector2::Zero);
				m_payloadRequest.payload->SetAlignedSize(Vector2::One);

				Widget* payloadRoot = GetSurfaceRenderer(PAYLOAD_WINDOW_SID)->GetWidgetManager().GetRoot();
				payloadRoot->AddChild(m_payloadRequest.payload);

				for (auto* l : m_payloadListeners)
					l->OnPayloadStarted(m_payloadRequest.type, m_payloadRequest.payload);
			}

			const auto& mp = GfxManager::GetLGX()->GetInput().GetMousePositionAbs();
			m_payloadWindow->SetPosition({static_cast<int32>(mp.x), static_cast<int32>(mp.y)});

			if (!GfxManager::GetLGX()->GetInput().GetMouseButton(LINAGX_MOUSE_0))
			{
				m_gfxManager->Join();

				m_payloadWindow->SetVisible(false);

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
					m_payloadRequest.payload->GetParent()->RemoveChild(m_payloadRequest.payload);

					if (m_payloadRequest.sourceWindow)
						m_payloadRequest.sourceWindow->BringToFront();

					if (m_payloadRequest.type == PayloadType::DockedPanel)
					{
						Panel* panel = static_cast<Panel*>(m_payloadRequest.payload);
						panel->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
						panel->GetFlags().Remove(WF_POS_ALIGN_Y);
						panel->SetAlignedPosX(0.0f);
						panel->SetAlignedSize(Vector2(1.0f, 0.0f));
						panel->SetAlignedPos(Vector2::Zero);
						Widget* panelArea = PrepareNewWindowToDock(m_subWindowCounter++, mp, panel->GetSize(), panel->GetDebugName());

						DockArea* dockArea = panelArea->GetWidgetManager()->Allocate<DockArea>("DockArea");
						dockArea->SetAlignedPos(Vector2::Zero);
						dockArea->SetAlignedSize(Vector2::One);
						panelArea->AddChild(dockArea);

						dockArea->AddPanel(panel);
					}
					else
						m_editor->GetEditorRoot()->GetWidgetManager()->Deallocate(m_payloadRequest.payload);
				}

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

	void WindowPanelManager::OpenPanel(PanelType type, StringID subData, Widget* requestingWidget)
	{
		// Go thru windows and try to find panel.
		auto findPanel = [type](const Vector<DockArea*>& areas) -> bool {
			for (auto* area : areas)
			{
				const auto& panels = area->GetPanels();

				for (auto* panel : panels)
				{
					if (panel->GetType() == type)
					{
						panel->GetWindow()->BringToFront();
						area->SetSelected(panel);
						return true;
					}
				}
			}
			return false;
		};

		// Check main
		Vector<DockArea*> primaryAreas;
		Widget::GetWidgetsOfType<DockArea>(primaryAreas, m_primaryWidgetManager->GetRoot());
		if (findPanel(primaryAreas))
			return;

		// Check subs
		for (auto* w : m_subWindows)
		{
			Vector<DockArea*> areas;
			SurfaceRenderer*  sf = GetSurfaceRenderer(static_cast<StringID>(w->GetSID()));
			Widget::GetWidgetsOfType<DockArea>(areas, sf->GetWidgetManager().GetRoot());
			if (findPanel(areas))
				return;
		}

		// Not found, create a window & insert panel.
		Vector2 pos = Vector2::Zero;

		if (requestingWidget)
			pos = requestingWidget->GetWindow()->GetPosition();
		else
			pos = m_mainWindow->GetPosition();

		Widget* panelArea = PrepareNewWindowToDock(m_subWindowCounter, pos, Vector2(500, 500), TO_STRING(m_subWindowCounter));
		m_subWindowCounter++;

		DockArea* dock = panelArea->GetWidgetManager()->Allocate<DockArea>("DockArea");
		dock->SetAlignedPos(Vector2::Zero);
		dock->SetAlignedSize(Vector2::One);
		panelArea->AddChild(dock);

		Panel* panel = PanelFactory::CreatePanel(dock, type, subData);
		dock->GetWindow()->SetTitle(panel->GetDebugName());
		dock->AddPanel(panel);
	}

	Widget* WindowPanelManager::PrepareNewWindowToDock(StringID sid, const Vector2& pos, const Vector2& size, const String& title)
	{
		const Vector2	usedSize = size.Clamp(m_editor->GetEditorRoot()->GetMonitorSize() * 0.1f, m_editor->GetEditorRoot()->GetMonitorSize());
		LinaGX::Window* window	 = m_gfxManager->CreateApplicationWindow(sid, title.c_str(), pos, usedSize, (uint32)LinaGX::WindowStyle::BorderlessApplication, m_mainWindow);
		CreateSurfaceRendererForWindow(window);

		m_subWindows.push_back(window);
		Widget*			   newWindowRoot = GetSurfaceRenderer(sid)->GetWidgetManager().GetRoot();
		DirectionalLayout* layout		 = newWindowRoot->GetWidgetManager()->Allocate<DirectionalLayout>("BaseLayout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		newWindowRoot->AddChild(layout);

		WindowBar* wb						= newWindowRoot->GetWidgetManager()->Allocate<WindowBar>("WindowBar");
		wb->GetBarProps().title				= "Lina Engine";
		wb->GetBarProps().hasIcon			= true;
		wb->GetBarProps().hasWindowButtons	= true;
		wb->GetBarProps().controlsDragRect	= true;
		wb->GetProps().backgroundStyle		= DirectionalLayout::BackgroundStyle::Default;
		wb->GetProps().colorBackgroundStart = Theme::GetDef().accentPrimary0;
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

	SurfaceRenderer* WindowPanelManager::GetSurfaceRenderer(StringID sid)
	{
		return m_surfaceRenderers.at(m_gfxManager->GetApplicationWindow(sid));
	}

	void WindowPanelManager::OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& size)
	{
		for (Pair<LinaGX::Window*, SurfaceRenderer*> pair : m_surfaceRenderers)
		{
			if (pair.first == window)
			{
				pair.second->OnWindowSizeChanged(window, size);
				break;
			}
		}
	}
} // namespace Lina::Editor
