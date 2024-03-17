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
#include "Core/Meta/ProjectSettings.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Editor/Widgets/Screens/SplashScreen.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Editor/Widgets/Popups/ProjectSelector.hpp"
#include "Editor/Widgets/Popups/InfoBar.hpp"
#include "Editor/Widgets/Testbed.hpp"
#include "Editor/Widgets/DockTestbed.hpp"

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

		// Load editor settings.
		const String userDataFolder = FileSystem::GetUserDataFolder();
		const String settingsPath	= userDataFolder + "EditorSettings.linameta";
		m_settings.SetPath(settingsPath);
		if (!FileSystem::FileOrPathExists(userDataFolder))
			FileSystem::CreateFolderInPath(userDataFolder);

		if (FileSystem::FileOrPathExists(settingsPath))
		{
			m_settings.LoadFromFile();
			OpenProject(m_settings.GetLastProjectPathBase(), m_settings.GetLastProjectName());
		}
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

		// Insert editor root.
		EditorRoot* editorRoot = root->Allocate<EditorRoot>("EditorRoot");
		editorRoot->Initialize();
		root->AddChild(editorRoot);

		if (m_currentProject == nullptr)
		{
			m_projectSelector = root->Allocate<ProjectSelector>("ProjectSelector");
			m_projectSelector->SetCancellable(false);
			m_projectSelector->SetTab(0);
			m_projectSelector->Initialize();
			m_primaryWidgetManager->AddToForeground(m_projectSelector);
			m_primaryWidgetManager->SetForegroundDim(0.5f);
		}
	}

	void Editor::Shutdown()
	{
	}

	void Editor::OpenProject(const String& basePath, const String& projectName)
	{
		const String projectFile = basePath + projectName + ".linaproject";
		if (!FileSystem::FileOrPathExists(projectFile))
			return;

		LINA_ASSERT(m_currentProject == nullptr, "");

		m_currentProject = new ProjectSettings();
		m_currentProject->SetPath(projectFile);
		m_currentProject->LoadFromFile();
	}

	void Editor::AddInfoBar(const String& text, LogLevel level, float time)
	{
		Widget* root = m_primaryWidgetManager->GetRoot();
		if (m_infoBar == nullptr)
		{
			m_infoBar = root->Allocate<InfoBar>("InfoBar");
			m_infoBar->Initialize();
			m_primaryWidgetManager->AddToForeground(m_infoBar);
		}

		m_infoBar->AddInfo(text, level, time);
	}

	void Editor::RemoveInfo()
	{
		if (m_infoBar == nullptr)
			return;
		m_infoBar->RemoveInfo();
	}

	void Editor::DestroyInfoBar()
	{
		if (m_infoBar == nullptr)
			return;
		Widget* root = m_primaryWidgetManager->GetRoot();
		m_primaryWidgetManager->RemoveFromForeground(m_infoBar);
		root->Deallocate(m_infoBar);
		m_infoBar = nullptr;
	}
} // namespace Lina::Editor
