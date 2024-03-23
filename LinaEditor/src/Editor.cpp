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
#include "Editor/Widgets/EditorRoot.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Editor/Widgets/Popups/ProjectSelector.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Editor/Widgets/Testbed.hpp"
#include "Editor/Widgets/DockTestbed.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"

namespace Lina::Editor
{

	void Editor::PreInitialize(const SystemInitializationInfo& initInfo)
	{
	}

	void Editor::Initialize(const SystemInitializationInfo& initInfo)
	{
		m_gfxManager		   = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		m_primaryWidgetManager = &m_gfxManager->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		// Push splash
		Widget*		  root	 = m_primaryWidgetManager->GetRoot();
		SplashScreen* splash = root->Allocate<SplashScreen>();
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

	void Editor::CoreResourcesLoaded()
	{
		// Remove splash
		Widget* root   = m_primaryWidgetManager->GetRoot();
		Widget* splash = root->GetChildren().at(0);
		root->RemoveChild(splash);
		root->Deallocate(splash);

		// Resize window to work dims.
		auto* window = m_gfxManager->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		// window->SetPosition(window->GetMonitorInfoFromWindow().workTopLeft);
		// window->AddSizeRequest(window->GetMonitorWorkSize());

		// Testbed* tb = root->Allocate<Testbed>();
		// root->AddChild(tb);
		// return;

		// Insert editor root.
		m_editorRoot = root->Allocate<EditorRoot>("EditorRoot");
		m_editorRoot->Initialize();
		root->AddChild(m_editorRoot);

		// We either load the last project, or load project selector in forced-mode.
		if (FileSystem::FileOrPathExists(m_settings.GetLastProjectPath()))
			OpenProject(m_settings.GetLastProjectPath());
		else
			OpenPopupProjectSelector(false);
	}

	void Editor::Shutdown()
	{
	}

	void Editor::OpenPopupProjectSelector(bool canCancel, bool openCreateFirst)
	{
		ProjectSelector* projectSelector = m_primaryWidgetManager->GetRoot()->Allocate<ProjectSelector>("ProjectSelector");
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
		RemoveCurrentProject();
		m_system->GetApp()->Quit();
	}

} // namespace Lina::Editor
