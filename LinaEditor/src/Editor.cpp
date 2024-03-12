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
#include "Editor/Widgets/Testbed.hpp"
#include "Editor/Widgets/DockTestbed.hpp"

namespace Lina::Editor
{
	void Editor::OnPreInitialize(Application* app)
	{
		m_app = app;

		// Push splash
		auto*		  gfxManager	= m_app->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		auto&		  widgetManager = gfxManager->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();
		SplashScreen* splash		= widgetManager.GetRoot()->Allocate<SplashScreen>();
		splash->Initialize();
		widgetManager.GetRoot()->AddChild(splash);

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

	void Editor::OnInitialize()
	{
		auto* gfxManager	= m_app->GetSystem()->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
		auto& widgetManager = gfxManager->GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		// Remove splash
		Widget* root   = widgetManager.GetRoot();
		Widget* splash = root->GetChildren().at(0);
		root->RemoveChild(splash);
		root->Deallocate(splash);

		// Resize window to work dims.
		auto* window = gfxManager->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		window->SetPosition(window->GetMonitorInfoFromWindow().workTopLeft);
		window->AddSizeRequest(window->GetMonitorWorkSize());

		// Testbed* tb = root->Allocate<Testbed>("Testbed");
		// DockTestbed* tb = root->Allocate<DockTestbed>("DockTestbed");
		// root->AddChild(tb);
		// return;

		// Insert editor root.
		EditorRoot* editorRoot = root->Allocate<EditorRoot>("EditorRoot");
		editorRoot->Initialize();
		root->AddChild(editorRoot);

		if (m_currentProject == nullptr)
		{
			m_projectSelector = root->Allocate<ProjectSelector>("ProjectSelector");
			m_projectSelector->Initialize();
			widgetManager.AddToForeground(m_projectSelector);
			widgetManager.SetForegroundDim(0.5f);
		}
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
} // namespace Lina::Editor
