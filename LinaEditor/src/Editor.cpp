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

#include "Editor/Editor.hpp"
#include "Core/Application.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Editor/Widgets/Screens/SplashScreen.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Panel/PanelFactory.hpp"
#include "Editor/Widgets/Panel/Panel.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Editor/Widgets/Popups/ProjectSelector.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Editor/Widgets/Testbed.hpp"
#include "Editor/Widgets/DockTestbed.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{

	void Editor::PreInitialize(const SystemInitializationInfo& initInfo)
	{
	}

	void Editor::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_settings.Initialize(this);
		m_layout.Initialize(this);

		m_gfxManager		   = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_primaryWidgetManager = &m_gfxManager->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		m_mainWindow	= m_gfxManager->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		m_payloadWindow = m_gfxManager->CreateApplicationWindow(PAYLOAD_WINDOW_SID, "Transparent", Vector2i(0, 0), Vector2(500, 500), (uint32)LinaGX::WindowStyle::BorderlessAlpha, m_mainWindow);
		m_payloadWindow->SetVisible(false);

		m_subWindows.push_back(m_payloadWindow);

		// Push splash
		Widget*		  root	 = m_primaryWidgetManager->GetRoot();
		SplashScreen* splash = root->GetWidgetManager()->Allocate<SplashScreen>();
		splash->Initialize();
		root->AddChild(splash);

		// Load editor settings or create and save empty if non-existing.
		const String userDataFolder = FileSystem::GetUserDataFolder();
		const String settingsPath	= userDataFolder + "EditorSettings.linameta";
		const String layoutPath		= userDataFolder + "EditorLayout.linameta";
		m_settings.SetPath(settingsPath);
		m_layout.SetPath(layoutPath);
		if (!FileSystem::FileOrPathExists(userDataFolder))
			FileSystem::CreateFolderInPath(userDataFolder);

		if (FileSystem::FileOrPathExists(settingsPath))
			m_settings.LoadFromFile();
		else
			m_settings.SaveToFile();

		if (FileSystem::FileOrPathExists(layoutPath))
			m_layout.LoadFromFile();
		else
			m_layout.SaveToFile();
	}

	void Editor::PreTick()
	{

		if (!m_windowCloseRequests.empty())
		{
			for (auto sid : m_windowCloseRequests)
			{
				auto it = linatl::find_if(m_subWindows.begin(), m_subWindows.end(), [sid](LinaGX::Window* w) -> bool { return static_cast<StringID>(w->GetSID()) == sid; });

				if (it != m_subWindows.end())
					m_subWindows.erase(it);
				m_gfxManager->DestroyApplicationWindow(sid);
			}

			m_windowCloseRequests.clear();
		}

		if (m_payloadRequest.active)
		{
			if (!m_payloadWindow->GetIsVisible())
			{
				m_payloadWindow->SetVisible(true);
				m_payloadWindow->SetAlpha(0.5f);
				m_payloadWindow->SetSize({static_cast<uint32>(m_payloadRequest.payload->GetSize().x), static_cast<uint32>(m_payloadRequest.payload->GetSize().y)});
				Widget* payloadRoot = m_gfxManager->GetSurfaceRenderer(PAYLOAD_WINDOW_SID)->GetWidgetManager().GetRoot();
				payloadRoot->AddChild(m_payloadRequest.payload);

				for (auto* l : m_payloadListeners)
					l->OnPayloadStarted(m_payloadRequest.type, m_payloadRequest.payload);
			}

			const auto& mp = m_gfxManager->GetLGX()->GetInput().GetMousePositionAbs();
			m_payloadWindow->SetPosition({static_cast<int32>(mp.x), static_cast<int32>(mp.y)});

			if (!m_gfxManager->GetLGX()->GetInput().GetMouseButton(LINAGX_MOUSE_0))
			{
				m_payloadWindow->SetVisible(false);
				m_payloadRequest.active = false;

				bool received = false;
				for (auto* l : m_payloadListeners)
				{
					if (l->OnPayloadDropped(m_payloadRequest.type, m_payloadRequest.payload))
					{
						received = true;
						break;
					}
				}

				for (auto* l : m_payloadListeners)
					l->OnPayloadEnded(m_payloadRequest.type, m_payloadRequest.payload);

				if (!received)
				{
					m_editorRoot->GetWidgetManager()->Deallocate(m_payloadRequest.payload);
				}
			}
		}
	}

	void Editor::CoreResourcesLoaded()
	{
		// Remove splash
		Widget* root   = m_primaryWidgetManager->GetRoot();
		Widget* splash = root->GetChildren().at(0);
		root->RemoveChild(splash);
		root->GetWidgetManager()->Deallocate(splash);

		// Resize window to work dims.
		// m_mainWindow->SetPosition(window->GetMonitorInfoFromWindow().workTopLeft);
		// m_mainWindow->AddSizeRequest(window->GetMonitorWorkSize());

		// Testbed* tb = root->Allocate<Testbed>();
		DockTestbed* tb = root->GetWidgetManager()->Allocate<DockTestbed>();
		root->AddChild(tb);
		tb->Initialize();
		return;

		// Insert editor root.
		m_editorRoot = root->GetWidgetManager()->Allocate<EditorRoot>("EditorRoot");
		m_editorRoot->Initialize();
		root->AddChild(m_editorRoot);

		// We either load the last project, or load project selector in forced-mode.
		if (FileSystem::FileOrPathExists(m_settings.GetLastProjectPath()))
			OpenProject(m_settings.GetLastProjectPath());
		else
			OpenPopupProjectSelector(false);
	}

	void Editor::PreShutdown()
	{
		for (auto* w : m_subWindows)
			m_gfxManager->DestroyApplicationWindow(static_cast<StringID>(w->GetSID()));

		m_subWindows.clear();

		m_settings.SaveToFile();
		RemoveCurrentProject();
	}

	void Editor::Shutdown()
	{
	}

	void Editor::OpenPopupProjectSelector(bool canCancel, bool openCreateFirst)
	{
		ProjectSelector* projectSelector = m_primaryWidgetManager->Allocate<ProjectSelector>("ProjectSelector");
		projectSelector->SetCancellable(canCancel);
		projectSelector->SetTab(openCreateFirst ? 0 : 1);
		projectSelector->Initialize();

		// When we select a project to open -> ask if we want to save current one if its dirty.
		projectSelector->GetProps().onProjectOpened = [this](const String& location) {
			if (m_isProjectDirty)
			{
				GenericPopup* popup = CommonWidgets::ThrowGenericPopup(Locale::GetStr(LocaleStr::UnfinishedWorkTitle), Locale::GetStr(LocaleStr::UnfinishedWorkDesc), m_primaryWidgetManager->GetRoot());

				popup->AddButton({.text = Locale::GetStr(LocaleStr::Yes), .onClicked = [&]() {
									  SaveProjectChanges();
									  RemoveCurrentProject();
									  OpenProject(location);
								  }});

				popup->AddButton({.text = Locale::GetStr(LocaleStr::No), .onClicked = [&]() {
									  RemoveCurrentProject();
									  OpenProject(location);
								  }});
			}
			else
			{
				RemoveCurrentProject();
				OpenProject(location);
			}
		};

		projectSelector->GetProps().onProjectCreated = [&](const String& path) {
			RemoveCurrentProject();

			if (m_isProjectDirty)
			{
				GenericPopup* popup = CommonWidgets::ThrowGenericPopup(Locale::GetStr(LocaleStr::UnfinishedWorkTitle), Locale::GetStr(LocaleStr::UnfinishedWorkDesc), m_primaryWidgetManager->GetRoot());

				// Save first then create & open.
				popup->AddButton({.text = Locale::GetStr(LocaleStr::Yes), .onClicked = [&]() {
									  SaveProjectChanges();
									  CreateEmptyProjectAndOpen(path);
								  }});

				// Create & open without saving
				popup->AddButton({.text = Locale::GetStr(LocaleStr::No), .onClicked = [&]() { CreateEmptyProjectAndOpen(path); }});
			}
			else
				CreateEmptyProjectAndOpen(path);
		};

		m_primaryWidgetManager->AddToForeground(projectSelector);
		m_primaryWidgetManager->SetForegroundDim(0.5f);
	}

	void Editor::SaveProjectChanges()
	{
		m_isProjectDirty = false;
		m_currentProject->SaveToFile();
		m_settings.SaveToFile();
	}

	void Editor::CreateEmptyProjectAndOpen(const String& path)
	{
		ProjectData dummy;
		dummy.SetPath(path);
		dummy.SetProjectName(FileSystem::GetFilenameOnlyFromPath(path));
		dummy.SaveToFile();
		OpenProject(path);
	}

	void Editor::RemoveCurrentProject()
	{
		if (m_currentProject == nullptr)
			return;

		delete m_currentProject;
		m_currentProject = nullptr;
	}

	void Editor::OpenProject(const String& projectFile)
	{
		LINA_ASSERT(m_currentProject == nullptr, "");
		m_currentProject = new ProjectData();
		m_currentProject->SetPath(projectFile);
		m_currentProject->LoadFromFile();
		m_editorRoot->SetProjectName(m_currentProject->GetProjectName());

		m_settings.SetLastProjectPath(projectFile);
		m_settings.SaveToFile();
	}

	void Editor::RequestExit()
	{
		m_system->GetApp()->Quit();
	}

	void Editor::AddPayloadListener(EditorPayloadListener* listener)
	{
		m_payloadListeners.push_back(listener);
	}

	void Editor::RemovePayloadListener(EditorPayloadListener* listener)
	{
		m_payloadListeners.erase(linatl::find(m_payloadListeners.begin(), m_payloadListeners.end(), listener));
	}

	void Editor::OpenPanel(PanelType type, StringID subData, Widget* requestingWidget)
	{
		// Go thru windows and try to find panel.

		// Create a window & insert panel.

		Vector2 pos = Vector2::Zero;

		if (requestingWidget)
			pos = requestingWidget->GetWindow()->GetPosition();
		else
			pos = m_mainWindow->GetPosition();

		LinaGX::Window* window = m_gfxManager->CreateApplicationWindow(static_cast<StringID>(type), "Lina Editor", pos, Vector2(500, 500), (uint32)LinaGX::WindowStyle::BorderlessApplication, m_mainWindow);
		m_subWindows.push_back(window);

		Widget* newWindowRoot = m_gfxManager->GetSurfaceRenderer(static_cast<StringID>(type))->GetWidgetManager().GetRoot();

		DirectionalLayout* layout = newWindowRoot->GetWidgetManager()->Allocate<DirectionalLayout>("BaseLayout");
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

		DockArea* dockArea = newWindowRoot->GetWidgetManager()->Allocate<DockArea>("DockArea");
		dockArea->SetAlignedPos(Vector2::Zero);
		dockArea->SetAlignedSize(Vector2::One);
		panelArea->AddChild(dockArea);

		Panel* panel = PanelFactory::CreatePanel(dockArea, type, subData);
		dockArea->AddPanel(panel);
	}

	void Editor::CloseWindow(StringID sid)
	{
		m_windowCloseRequests.push_back(sid);
	}

	void Editor::CreatePayload(Widget* payload, PayloadType type)
	{
		m_payloadRequest.active	 = true;
		m_payloadRequest.payload = payload;
		m_payloadRequest.type	 = type;
	}

} // namespace Lina::Editor
