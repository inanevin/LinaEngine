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

#pragma once

#include "Core/ApplicationDelegate.hpp"
#include "Common/Data/HashMap.hpp"
#include "Meta/EditorSettings.hpp"
#include "Editor/CommonEditor.hpp"
#include "Atlas/AtlasManager.hpp"
#include "Editor/WindowPanelManager.hpp"
#include "Editor/EditorTaskManager.hpp"
#include "Editor/World/EditorWorldManager.hpp"
#include "Editor/Project/ProjectManager.hpp"
#include "Editor/Graphics/EditorRenderer.hpp"
#include "Editor/Actions/EditorActionManager.hpp"
#include "Editor/Resources/EditorResources.hpp"

namespace Lina
{
	class Application;
	class WidgetManager;
	class EntityWorld;
	class Widget;
	class GUIWidget;
	class WorldRenderer;
} // namespace Lina

namespace LinaGX
{
	class Window;
}

namespace Lina::Editor
{
	class SplashScreen;
	class EditorRoot;
	class DockArea;
	class WorldRendererExtEditor;

	class Editor : public ApplicationDelegate, public LinaGX::WindowListener
	{

	private:
	public:
		Editor(){};
		virtual ~Editor() = default;
		static Editor* Get()
		{
			return s_editor;
		}

		// Application delegate
		virtual bool	PreInitialize() override;
		virtual bool	Initialize() override;
		virtual void	PreTick() override;
		virtual void	Tick(float delta) override;
		virtual void	PreShutdown() override;
		virtual void	SyncRender() override;
		virtual void	Render(uint32 frameIndex) override;
		virtual void	OnWindowClose(LinaGX::Window* window) override;
		virtual void	OnWindowSizeChanged(LinaGX::Window* window, const LinaGX::LGXVector2ui&) override;
		virtual void	OnWindowKey(LinaGX::Window* window, uint32 keycode, int32 scancode, LinaGX::InputAction inputAction) override;
		virtual void	OnWindowMouse(LinaGX::Window* window, uint32 button, LinaGX::InputAction inputAction) override;
		virtual void	OnWindowMouseWheel(LinaGX::Window* window, float amt) override;
		virtual void	OnWindowMouseMove(LinaGX::Window* window, const LinaGX::LGXVector2&) override;
		virtual void	OnWindowFocus(LinaGX::Window* window, bool gainedFocus) override;
		virtual void	OnWindowHoverBegin(LinaGX::Window* window) override;
		virtual void	OnWindowHoverEnd(LinaGX::Window* window) override;
		LinaGX::Window* CreateEditorWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2ui& size, uint32 style, LinaGX::Window* parentWindow);
		void			DestroyEditorWindow(LinaGX::Window* window);

		// Misc
		void SaveSettings();
		void RequestExit();

		// Play
		void SetIsPlaying(bool isPlaying);

		inline EntityWorld* GetCurrentWorld() const
		{
			return m_currentWorld;
		}

		inline EditorRoot* GetEditorRoot() const
		{
			return m_editorRoot;
		}

		inline EditorSettings& GetSettings()
		{
			return m_settings;
		}

		inline AtlasManager& GetAtlasManager()
		{
			return m_atlasManager;
		}

		inline WindowPanelManager& GetWindowPanelManager()
		{
			return m_windowPanelManager;
		}

		inline ProjectManager& GetProjectManager()
		{
			return m_projectManager;
		}

		inline EditorRenderer& GetEditorRenderer()
		{
			return m_editorRenderer;
		}

		inline EditorActionManager& GetEditorActionManager()
		{
			return m_editorActionManager;
		}

		inline EditorTaskManager& GetTaskManager()
		{
			return m_taskManager;
		}

		inline EditorWorldManager& GetWorldManager()
		{
			return m_worldManager;
		}

	private:
		EditorActionManager m_editorActionManager;
		EditorRenderer		m_editorRenderer;
		WindowPanelManager	m_windowPanelManager;
		AtlasManager		m_atlasManager;
		ProjectManager		m_projectManager;
		EditorSettings		m_settings			   = {};
		WidgetManager*		m_primaryWidgetManager = nullptr;
		EntityWorld*		m_currentWorld		   = nullptr;
		EditorRoot*			m_editorRoot		   = nullptr;
		LinaGX::Window*		m_mainWindow		   = nullptr;
		static Editor*		s_editor;
		EditorTaskManager	m_taskManager;
		EditorResources		m_editorResources;
		EditorWorldManager	m_worldManager;
		bool				m_isPlaying = false;
	};

} // namespace Lina::Editor
