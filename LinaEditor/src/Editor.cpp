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
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Editor/Meta/ProjectData.hpp"
#include "Editor/Widgets/Screens/SplashScreen.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Panel/PanelFactory.hpp"
#include "Editor/Widgets/Panel/Panel.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Widgets/Popups/ProjectSelector.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Editor/Widgets/Testbed.hpp"
#include "Editor/Widgets/DockTestbed.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/Math/Math.hpp"
#include "Core/CommonCore.hpp"

#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"

#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	SystemInitializationInfo Lina_GetInitInfo()
	{
		LinaGX::MonitorInfo monitor = LinaGX::Window::GetPrimaryMonitorInfo();

		const uint32 w = monitor.size.x / 4;
		const uint32 h = static_cast<uint32>(static_cast<float>(w) * (static_cast<float>(monitor.size.y) / static_cast<float>(monitor.size.x)));

		LinaGX::VSyncStyle vsync;
		vsync.dx12Vsync	  = LinaGX::DXVsync::None;
		vsync.vulkanVsync = LinaGX::VKVsync::None;

		return SystemInitializationInfo{
			.appName					 = "Lina Editor",
			.windowWidth				 = w,
			.windowHeight				 = h,
			.windowStyle				 = LinaGX::WindowStyle::BorderlessApplication,
			.vsyncStyle					 = vsync,
			.allowTearing				 = true,
			.appDelegate				 = new Lina::Editor::Editor(),
			.resourceManagerMode		 = Lina::ResourceManagerMode::File,
			.resourceManagerUseMetacache = false,
			.clearColor					 = Theme::GetDef().background0,
		};
	}
} // namespace Lina

namespace Lina::Editor
{
	Editor* Editor::s_editor = nullptr;

	bool Editor::FillResourceCustomMeta(StringID sid, OStream& stream)
	{
		if (sid == ICON_FONT_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 32, .dpiLimit = 10.0f}},
				.isSDF	= true,
			};
			customMeta.SaveToStream(stream);
			return true;
		}

		if (sid == ALT_FONT_SID || sid == ALT_FONT_BOLD_SID)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 14, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 16, .dpiLimit = 10.0f}},
				.isSDF	= false,
			};
			customMeta.SaveToStream(stream);
			return true;
		}
		return false;
	}

	void Editor::RegisterAppResources(ResourceManager& rm)
	{
		Vector<ResourceIdentifier> resources;

		// Priority
		resources.push_back(ResourceIdentifier(ICON_FONT_PATH, GetTypeID<Font>(), 0, true, ResourceTag::Priority));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/LinaLogoTitle.png", GetTypeID<Texture>(), 0, true, ResourceTag::Priority));

		// Core
		resources.push_back(ResourceIdentifier(ALT_FONT_PATH, GetTypeID<Font>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier(ALT_FONT_BOLD_PATH, GetTypeID<Font>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/LinaLogoTitleHorizontal.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/Test.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/Test1.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/Test2.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/Test3.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/Test4.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/Test5.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/Test6.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		resources.push_back(ResourceIdentifier("Resources/Editor/Textures/Test7.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));

		for (auto& r : resources)
			r.sid = TO_SID(r.path);

		rm.RegisterAppResources(resources);
	}

	void Editor::Initialize()
	{
		s_editor							  = this;
		Theme::GetDef().iconFont			  = ICON_FONT_SID;
		Theme::GetDef().defaultFont			  = DEFAULT_FONT_SID;
		Theme::GetDef().altFont				  = ALT_FONT_BOLD_SID;
		Theme::GetDef().iconDropdown		  = ICON_ARROW_DOWN;
		Theme::GetDef().iconSliderHandle	  = ICON_CIRCLE_FILLED;
		Theme::GetDef().iconColorWheelPointer = ICON_CIRCLE;

		m_fileManager.Initialize(this);

		m_gfxManager		   = m_app->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
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
		m_settings.SetPath(settingsPath);
		if (!FileSystem::FileOrPathExists(userDataFolder))
			FileSystem::CreateFolderInPath(userDataFolder);

		if (FileSystem::FileOrPathExists(settingsPath))
			m_settings.LoadFromFile();
		else
			m_settings.SaveToFile();
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
				m_payloadWindow->SetSize(m_payloadRequest.size.AsLGX2UI());

				m_payloadRequest.payload->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
				m_payloadRequest.payload->SetAlignedPos(Vector2::Zero);
				m_payloadRequest.payload->SetAlignedSize(Vector2::One);

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
						m_editorRoot->GetWidgetManager()->Deallocate(m_payloadRequest.payload);
				}

				m_payloadRequest = {};
			}
		}
	}

	void Editor::Tick(float delta)
	{
	}

	void Editor::CoreResourcesLoaded()
	{
		// Remove splash
		Widget* root   = m_primaryWidgetManager->GetRoot();
		Widget* splash = root->GetChildren().at(0);
		root->RemoveChild(splash);
		root->GetWidgetManager()->Deallocate(splash);

		// Resize window to work dims.
		m_mainWindow->SetPosition(m_mainWindow->GetMonitorInfoFromWindow().workTopLeft);
		m_mainWindow->AddSizeRequest(m_mainWindow->GetMonitorWorkSize());

		// Testbed* tb = root->GetWidgetManager()->Allocate<Testbed>();
		// // DockTestbed* tb = root->GetWidgetManager()->Allocate<DockTestbed>();
		// root->AddChild(tb);
		// tb->Initialize();

		// Insert editor root.
		m_editorRoot = root->GetWidgetManager()->Allocate<EditorRoot>("EditorRoot");
		m_editorRoot->Initialize();
		root->AddChild(m_editorRoot);

		// We either load the last project, or load project selector in forced-mode.
		if (FileSystem::FileOrPathExists(m_settings.GetLastProjectPath()))
			OpenProject(m_settings.GetLastProjectPath());
		else
			OpenPopupProjectSelector(false);

		m_settings.GetLayout().ApplyStoredLayout();
	}

	void Editor::PreShutdown()
	{
		m_fileManager.Shutdown();

		for (auto* w : m_subWindows)
			m_gfxManager->DestroyApplicationWindow(static_cast<StringID>(w->GetSID()));

		m_subWindows.clear();

		m_settings.SaveToFile();
		RemoveCurrentProject();
	}

	void Editor::OpenPopupProjectSelector(bool canCancel, bool openCreateFirst)
	{
		ProjectSelector* projectSelector = m_primaryWidgetManager->Allocate<ProjectSelector>("ProjectSelector");
		projectSelector->SetCancellable(canCancel);
		projectSelector->SetTab(openCreateFirst ? 0 : 1);
		projectSelector->Initialize();

		// When we select a project to open -> ask if we want to save current one if its dirty.
		projectSelector->GetProps().onProjectOpened = [this](const String& location) {
			if (m_currentProject && m_currentProject->GetIsDirty())
			{
				GenericPopup* popup = CommonWidgets::ThrowGenericPopup(Locale::GetStr(LocaleStr::UnfinishedWorkTitle), Locale::GetStr(LocaleStr::UnfinishedWorkDesc), m_primaryWidgetManager->GetRoot());

				m_primaryWidgetManager->AddToForeground(popup);

				popup->AddButton({.text = Locale::GetStr(LocaleStr::Yes), .onClicked = [location, popup, this]() {
									  SaveProjectChanges();
									  RemoveCurrentProject();
									  OpenProject(location);
									  m_primaryWidgetManager->RemoveFromForeground(popup);
									  m_primaryWidgetManager->Deallocate(popup);
								  }});

				popup->AddButton({.text = Locale::GetStr(LocaleStr::No), .onClicked = [location, popup, this]() {
									  RemoveCurrentProject();
									  OpenProject(location);
									  m_primaryWidgetManager->RemoveFromForeground(popup);
									  m_primaryWidgetManager->Deallocate(popup);
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

			if (m_currentProject && m_currentProject->GetIsDirty())
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
		SaveSettings();

		m_currentProject->SetDirty(false);
		m_currentProject->SaveToFile();
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
		m_currentProject->Initialize(this);
		m_currentProject->LoadFromFile();
		m_editorRoot->SetProjectName(m_currentProject->GetProjectName());

		m_settings.SetLastProjectPath(projectFile);
		m_settings.SaveToFile();
		m_fileManager.SetProjectDirectory(FileSystem::GetFilePath(projectFile));
		m_fileManager.RefreshResources();

		const String& lastWorldPath = m_settings.GetLastWorldAbsPath();
		if (FileSystem::FileOrPathExists(lastWorldPath))
			m_app->GetSystem()->CastSubsystem<WorldManager>(SubsystemType::WorldManager)->InstallWorld(lastWorldPath);
	}

	void Editor::RequestExit()
	{
		SaveSettings();
		m_app->Quit();
	}

	void Editor::SaveSettings()
	{
		auto* loadedWorld = m_app->GetSystem()->CastSubsystem<WorldManager>(SubsystemType::WorldManager)->GetMainWorld();
		if (loadedWorld)
			m_settings.SetLastWorldAbsPath(loadedWorld->GetPath());

		m_settings.GetLayout().StoreLayout();
		m_settings.SaveToFile();
	}

	void Editor::AddPayloadListener(EditorPayloadListener* listener)
	{
		m_payloadListeners.push_back(listener);
	}

	void Editor::RemovePayloadListener(EditorPayloadListener* listener)
	{
		m_payloadListeners.erase(linatl::find(m_payloadListeners.begin(), m_payloadListeners.end(), listener));
	}

	void Editor::CreatePayload(Widget* payload, PayloadType type, const Vector2ui& size)
	{
		m_payloadRequest.active		  = true;
		m_payloadRequest.payload	  = payload;
		m_payloadRequest.type		  = type;
		m_payloadRequest.size		  = size;
		m_payloadRequest.sourceWindow = payload->GetWindow();
	}

	void Editor::OpenPanel(PanelType type, StringID subData, Widget* requestingWidget)
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
			Widget::GetWidgetsOfType<DockArea>(areas, m_gfxManager->GetSurfaceRenderer(static_cast<StringID>(w->GetSID()))->GetWidgetManager().GetRoot());
			if (findPanel(areas))
				return;
		}

		// Not found, create a window & insert panel.
		Vector2 pos = Vector2::Zero;

		if (requestingWidget)
			pos = requestingWidget->GetWindow()->GetPosition();
		else
			pos = m_mainWindow->GetPosition();

		Widget* panelArea = PrepareNewWindowToDock(m_subWindowCounter++, pos, Vector2(500, 500), TO_STRING(m_subWindowCounter));

		DockArea* dock = panelArea->GetWidgetManager()->Allocate<DockArea>("DockArea");
		dock->SetAlignedPos(Vector2::Zero);
		dock->SetAlignedSize(Vector2::One);
		panelArea->AddChild(dock);

		Panel* panel = PanelFactory::CreatePanel(dock, type, subData);
		dock->GetWindow()->SetTitle(panel->GetDebugName());
		dock->AddPanel(panel);
	}

	Widget* Editor::PrepareNewWindowToDock(StringID sid, const Vector2& pos, const Vector2& size, const String& title)
	{
		const Vector2	usedSize = size.Clamp(m_editorRoot->GetMonitorSize() * 0.1f, m_editorRoot->GetMonitorSize());
		LinaGX::Window* window	 = m_gfxManager->CreateApplicationWindow(sid, title.c_str(), pos, usedSize, (uint32)LinaGX::WindowStyle::BorderlessApplication, m_mainWindow);
		m_subWindows.push_back(window);
		Widget*			   newWindowRoot = m_gfxManager->GetSurfaceRenderer(sid)->GetWidgetManager().GetRoot();
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

	void Editor::CloseAllSubwindows()
	{
		for (auto* w : m_subWindows)
			CloseWindow(static_cast<StringID>(w->GetSID()));
	}

	void Editor::CloseWindow(StringID sid)
	{
		m_windowCloseRequests.push_back(sid);
	}

} // namespace Lina::Editor
