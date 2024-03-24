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

#include "Meta/EditorSettings.hpp"
#include "Editor/Meta/EditorLayout.hpp"
#include "Editor/CommonEditor.hpp"
#include "Common/System/Subsystem.hpp"

namespace Lina
{
	class ProjectData;
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

	class EditorPayloadListener
	{
	public:
		virtual void OnPayloadStarted(PayloadType type, Widget* payload)
		{
			return false;
		}
		virtual void OnPayloadEnded(PayloadType type, Widget* payload)
		{
			return false;
		}
		virtual bool OnPayloadDropped(PayloadType type, Widget* payload)
		{
			return false;
		}
	};

	class Editor : public Subsystem
	{
	public:
		struct PayloadRequest
		{
			Widget*		payload = nullptr;
			PayloadType type	= PayloadType::DockedPanel;
			bool		active	= false;
			Vector2ui	size	= Vector2ui::Zero;
		};

		Editor(System* sys) : Subsystem(sys, SubsystemType::Editor){};
		virtual ~Editor() = default;

		virtual void PreInitialize(const SystemInitializationInfo& initInfo) override;
		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void PreTick() override;
		virtual void CoreResourcesLoaded() override;
		virtual void PreShutdown() override;
		virtual void Shutdown() override;

		// Project
		void OpenPopupProjectSelector(bool canCancel, bool openCreateFirst = true);
		void OpenProject(const String& projectFile);
		void SaveProjectChanges();
		void CloseCurrentProject();

		// Payload && Panel & Windows
		void	  AddPayloadListener(EditorPayloadListener* listener);
		void	  RemovePayloadListener(EditorPayloadListener* listener);
		void	  OpenPanel(PanelType type, StringID subData, Widget* requestingWidget);
		DockArea* PrepareNewWindowToDock(StringID sid, const Vector2& pos, const Vector2& size, const String& title);
		void	  CreatePayload(Widget* payload, PayloadType type);
		void	  CloseWindow(StringID sid);

		// Misc
		void RequestExit();

		inline void SetIsProjectDirty(bool isDirty)
		{
			m_isProjectDirty = isDirty;
		}

		inline void SetIsWorldDirty(bool isDirty)
		{
			m_isWorldDirty = isDirty;
		}

		inline bool GetIsProjectDirty() const
		{
			return m_isProjectDirty;
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

	private:
		void RemoveCurrentProject();
		void CreateEmptyProjectAndOpen(const String& path);

	private:
		GfxManager*					   m_gfxManager			  = nullptr;
		WidgetManager*				   m_primaryWidgetManager = nullptr;
		EditorSettings				   m_settings			  = {};
		EditorLayout				   m_layout				  = {};
		ProjectData*				   m_currentProject		  = nullptr;
		EntityWorld*				   m_currentWorld		  = nullptr;
		bool						   m_isWorldDirty		  = false;
		bool						   m_isProjectDirty		  = false;
		EditorRoot*					   m_editorRoot			  = nullptr;
		Vector<LinaGX::Window*>		   m_subWindows			  = {};
		Vector<StringID>			   m_windowCloseRequests;
		PayloadRequest				   m_payloadRequest;
		LinaGX::Window*				   m_payloadWindow = nullptr;
		LinaGX::Window*				   m_mainWindow	   = nullptr;
		Vector<EditorPayloadListener*> m_payloadListeners;
		StringID					   m_subWindowCounter = 0;
	};

} // namespace Lina::Editor
