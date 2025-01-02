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
#include "Editor/EditorLocale.hpp"
#include "Editor/Resources/EditorResources.hpp"
#include "Editor/Resources/ShaderImport.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/IO/ExtensionSupport.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/Application.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/CommonCore.hpp"
#include "Core/Graphics/Utility/TextureAtlas.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	SystemInitializationInfo Lina_GetInitInfo()
	{
		LinaGX::MonitorInfo monitor = LinaGX::Window::GetPrimaryMonitorInfo();

		const uint32 w = monitor.size.x / 4;
		const uint32 h = static_cast<uint32>(static_cast<float>(w) * (static_cast<float>(monitor.size.y) / static_cast<float>(monitor.size.x)));

		LinaGX::VSyncStyle vsync;
		vsync.dx12Vsync	  = LinaGX::DXVsync::EveryVBlank;
		vsync.vulkanVsync = LinaGX::VKVsync::FIFO;

		return SystemInitializationInfo{
			.appName	  = "Lina Editor",
			.windowWidth  = w,
			.windowHeight = h,
			.windowStyle  = WindowStyle::BorderlessApplication,
			.appDelegate  = new Lina::Editor::Editor(),
		};
	}
} // namespace Lina

namespace Lina::Editor
{
	Editor* Editor::s_editor = nullptr;

	bool Editor::PreInitialize()
	{
		Shader::SetVariantInjectionCallback(ShaderImport::InjectShaderVariant);

		if (!m_editorResources.LoadPriorityResources(m_app->GetResourceManager()))
		{
			LINA_ERR("Loading priority editor resources went bad, aborting!");
			return false;
		}

		m_taskManager.Initialize(this);
		m_editorRenderer.Initialize(this);

		return true;
	}

	bool Editor::Initialize()
	{
		s_editor = this;
		m_app->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->AddListener(this);

		Theme::GetDef().iconFont		 = EDITOR_FONT_ICON_ID;
		Theme::GetDef().defaultFont		 = EDITOR_FONT_ROBOTO_ID;
		Theme::GetDef().defaultBoldFont	 = EDITOR_FONT_ROBOTO_BOLD_ID;
		Theme::GetDef().altFont			 = EDITOR_FONT_PLAY_BOLD_ID;
		Theme::GetDef().altBigFont		 = EDITOR_FONT_PLAY_BIG_ID;
		Theme::GetDef().iconDropdown	 = ICON_ARROW_DOWN;
		Theme::GetDef().iconCheckbox	 = ICON_CHECK;
		Theme::GetDef().iconCircleFilled = ICON_CIRCLE_FILLED;
		Theme::GetDef().iconSliderHandle = ICON_CIRCLE_FILLED;

		ExtensionSupport::AddTypeInfoToReflection();
		m_atlasManager.Initialize(this);
		m_windowPanelManager.Initialize(this);
		m_editorActionManager.Initialize(this);
		m_worldManager.Initialize(this);

		m_mainWindow		   = m_app->GetApplicationWindow(LINA_MAIN_SWAPCHAIN);
		m_primaryWidgetManager = &m_windowPanelManager.GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		EditorTask* task   = m_taskManager.CreateTask();
		task->title		   = Locale::GetStr(LocaleStr::LoadingEngine);
		task->progressText = Locale::GetStr(LocaleStr::LoadingSettings);
		task->ownerWindow  = m_mainWindow;

		m_editorResources.StartLoadCoreResources(GetApp()->GetResourceManager());

		task->task = [this, task]() {
			// Load editor settings or create and save empty if non-existing.
			const String userDataFolder = FileSystem::GetUserDataFolder() + "Editor/";
			const String settingsPath	= userDataFolder + "EditorSettings.linameta";
			m_settings.SetPath(settingsPath);
			if (!FileSystem::FileOrPathExists(userDataFolder))
				FileSystem::CreateFolderInPath(userDataFolder);
			if (FileSystem::FileOrPathExists(settingsPath))
			{
				if (!m_settings.LoadFromFile())
					m_settings.SaveToFile();
			}
			else
				m_settings.SaveToFile();

			task->progressText = Locale::GetStr(LocaleStr::LoadingCoreResources);
			m_editorResources.LoadCoreResources();

			task->progressText = Locale::GetStr(LocaleStr::GeneratingAtlases);
			m_atlasManager.ScanCustomAtlas("ProjectIcons"_hs, Vector2ui(2048, 2048), "Resources/Editor/Textures/Atlas/ProjectIcons/");
		};

		task->onComplete = [this]() {
			m_atlasManager.RefreshAtlas("ProjectIcons"_hs);

			if (!m_editorResources.EndLoadCoreResources(m_app->GetGfxContext()))
			{
				m_app->Quit("Failed loading editor core resources!");
				return;
			}

			// Resize window to work dims.
			m_mainWindow->SetPosition(m_mainWindow->GetMonitorInfoFromWindow().workTopLeft);
			m_mainWindow->AddSizeRequest(m_mainWindow->GetMonitorWorkSize());

			// Insert editor root.
			Widget* root = m_primaryWidgetManager->GetRoot();
			m_editorRoot = root->GetWidgetManager()->Allocate<EditorRoot>("EditorRoot");
			m_editorRoot->Initialize();
			root->AddChild(m_editorRoot);

			// Launch project
			m_projectManager.Initialize(this);
		};

		m_taskManager.AddTask(task);

		return true;
	}

	void Editor::OnWindowKey(LinaGX::Window* window, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{
		SurfaceRenderer* rend = m_windowPanelManager.GetSurfaceRenderer(window->GetSID());

		LinaGX::Input& lgxInput = m_app->GetLGX()->GetInput();
		if (keycode == LINAGX_KEY_Z && inputAction != LinaGX::InputAction::Released && lgxInput.GetKey(LINAGX_KEY_LCTRL))
		{
			m_editorActionManager.Undo();
			return;
		}

		if (keycode == LINAGX_KEY_Y && inputAction != LinaGX::InputAction::Released && lgxInput.GetKey(LINAGX_KEY_LCTRL))
		{
			m_editorActionManager.Redo();
			return;
		}

		rend->GetWidgetManager().OnKey(keycode, scancode, inputAction);
	}

	void Editor::OnWindowMouse(LinaGX::Window* window, uint32 button, LinaGX::InputAction inputAction)
	{
		SurfaceRenderer* rend = m_windowPanelManager.GetSurfaceRenderer(window->GetSID());
		rend->GetWidgetManager().OnMouse(button, inputAction);
	}

	void Editor::OnWindowMouseWheel(LinaGX::Window* window, float amt)
	{
		SurfaceRenderer* rend = m_windowPanelManager.GetSurfaceRenderer(window->GetSID());
		rend->GetWidgetManager().OnMouseWheel(amt);
	}

	void Editor::OnWindowMouseMove(LinaGX::Window* window, const LinaGX::LGXVector2& pos)
	{
		SurfaceRenderer* rend = m_windowPanelManager.GetSurfaceRenderer(window->GetSID());
		rend->GetWidgetManager().OnMouseMove(pos);
	}

	void Editor::OnWindowFocus(LinaGX::Window* window, bool gainedFocus)
	{
		SurfaceRenderer* rend = m_windowPanelManager.GetSurfaceRenderer(window->GetSID());
		rend->GetWidgetManager().OnFocus(gainedFocus);
	}

	void Editor::OnWindowHoverBegin(LinaGX::Window* window)
	{
		SurfaceRenderer* rend = m_windowPanelManager.GetSurfaceRenderer(window->GetSID());
		rend->GetWidgetManager().OnHoverBegin();
		;
	}

	void Editor::OnWindowHoverEnd(LinaGX::Window* window)
	{
		SurfaceRenderer* rend = m_windowPanelManager.GetSurfaceRenderer(window->GetSID());
		rend->GetWidgetManager().OnHoverEnd();
	}

	LinaGX::Window* Editor::CreateEditorWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow)
	{
		LinaGX::Window* w = m_app->CreateApplicationWindow(sid, title, pos, size, style, parentWindow);
		w->AddListener(this);
		return w;
	}

	void Editor::DestroyEditorWindow(LinaGX::Window* window)
	{
		window->RemoveListener(this);
		m_app->DestroyApplicationWindow(window->GetSID());
	}

	void Editor::OnWindowClose(LinaGX::Window* window)
	{
		m_app->Quit();
	}

	void Editor::OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui& size)
	{
		m_app->JoinRender();
		m_windowPanelManager.GetSurfaceRenderer(window->GetSID())->OnWindowSizeChanged(window, size);
	}

	void Editor::PreTick()
	{
		m_projectManager.PreTick();
		m_taskManager.PreTick();
		m_windowPanelManager.PreTick();
		m_editorRenderer.PreTick();
	}

	void Editor::Tick(float delta)
	{
		m_worldManager.Tick(delta);
		m_editorRenderer.Tick(delta);
	}

	void Editor::SyncRender()
	{
		m_editorRenderer.SyncRender();
	}

	void Editor::Render(uint32 frameIndex)
	{
		m_editorRenderer.Render(frameIndex);
	}

	void Editor::PreShutdown()
	{
		m_worldManager.Shutdown();
		m_editorActionManager.Shutdown();
		m_editorRenderer.Shutdown();
		m_atlasManager.Shutdown();
		m_windowPanelManager.Shutdown();
		m_projectManager.Shutdown();

		m_app->GetApplicationWindow(LINA_MAIN_SWAPCHAIN)->RemoveListener(this);
	}

	void Editor::RequestExit()
	{
		SaveSettings();
		m_app->Quit();
	}

	void Editor::SaveSettings()
	{
		m_settings.GetLayout().StoreLayout();
		m_settings.SaveToFile();
	}

} // namespace Lina::Editor
