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
#include "Meta/EditorSettings.hpp"
#include "Editor/CommonEditor.hpp"
#include "IO/FileManager.hpp"

namespace Lina
{
	class Application;
	class GfxManager;
	class WidgetManager;
	class EntityWorld;
	class Widget;
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
	class ProjectData;

	class EditorPayloadListener
	{
	public:
		virtual void OnPayloadStarted(PayloadType type, Widget* payload)
		{
		}
		virtual void OnPayloadEnded(PayloadType type, Widget* payload)
		{
		}
		virtual bool OnPayloadDropped(PayloadType type, Widget* payload)
		{
			return false;
		}

		virtual LinaGX::Window* OnPayloadGetWindow()
		{
			return nullptr;
		}
	};

	class Editor : public ApplicationDelegate
	{
	public:
		struct PayloadRequest
		{
			Widget*			payload		 = nullptr;
			LinaGX::Window* sourceWindow = nullptr;
			PayloadType		type		 = PayloadType::DockedPanel;
			bool			active		 = false;
			Vector2ui		size		 = Vector2ui::Zero;
		};

		Editor(){};
		virtual ~Editor() = default;
		static Editor* Get()
		{
			return s_editor;
		}

		// Application delegate
		virtual void   Initialize() override;
		virtual void   PreTick() override;
		virtual void   Tick(float delta) override;
		virtual void   CoreResourcesLoaded() override;
		virtual void   PreShutdown() override;
		virtual bool   FillResourceCustomMeta(StringID sid, OStream& stream) override;
		virtual void   RegisterAppResources(ResourceManager& rm) override;
		virtual String GetBaseMetacachePath() const override
		{
			return "Resources/Editor/Metacache/";
		}

		// Project
		void OpenPopupProjectSelector(bool canCancel, bool openCreateFirst = true);
		void OpenProject(const String& projectFile);
		void SaveProjectChanges();
		void CloseCurrentProject();

		// Payload
		void AddPayloadListener(EditorPayloadListener* listener);
		void RemovePayloadListener(EditorPayloadListener* listener);
		void CreatePayload(Widget* payload, PayloadType type, const Vector2ui& size);

		// Panel and windows
		void	OpenPanel(PanelType type, StringID subData, Widget* requestingWidget);
		Widget* PrepareNewWindowToDock(StringID sid, const Vector2& pos, const Vector2& size, const String& title);
		void	CloseWindow(StringID sid);
		void	CloseAllSubwindows();

		// Misc
		void SaveSettings();
		void RequestExit();

		inline void SetIsWorldDirty(bool isDirty)
		{
			m_isWorldDirty = isDirty;
		}

		inline bool GetIsWorldDirty() const
		{
			return m_isWorldDirty;
		}

		inline ProjectData* GetProjectData() const
		{
			return m_currentProject;
		}

		inline EntityWorld* GetCurrentWorld() const
		{
			return m_currentWorld;
		}

		inline EditorRoot* GetEditorRoot() const
		{
			return m_editorRoot;
		}

		inline const Vector<LinaGX::Window*>& GetSubWindows() const
		{
			return m_subWindows;
		}

		inline EditorSettings& GetSettings()
		{
			return m_settings;
		}

		inline const FileManager& GetFileManager() const
		{
			return m_fileManager;
		}

	private:
		void RemoveCurrentProject();
		void CreateEmptyProjectAndOpen(const String& path);

	private:
		GfxManager*					   m_gfxManager			  = nullptr;
		WidgetManager*				   m_primaryWidgetManager = nullptr;
		EditorSettings				   m_settings			  = {};
		ProjectData*				   m_currentProject		  = nullptr;
		EntityWorld*				   m_currentWorld		  = nullptr;
		bool						   m_isWorldDirty		  = false;
		EditorRoot*					   m_editorRoot			  = nullptr;
		Vector<LinaGX::Window*>		   m_subWindows			  = {};
		Vector<StringID>			   m_windowCloseRequests;
		PayloadRequest				   m_payloadRequest;
		LinaGX::Window*				   m_payloadWindow = nullptr;
		LinaGX::Window*				   m_mainWindow	   = nullptr;
		Vector<EditorPayloadListener*> m_payloadListeners;
		StringID					   m_subWindowCounter = 0;
		FileManager					   m_fileManager;
		static Editor*				   s_editor;
	};

} // namespace Lina::Editor
