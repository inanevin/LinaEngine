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

#include "Core/Resources/CommonResources.hpp"
#include "Core/System/PluginInterface.hpp"

namespace LinaGX
{
	class Window;
}

namespace Lina
{
	class ProjectData;
	class WidgetManager;
	struct ResourceDirectory;
	struct TextureAtlasImage;
	class Widget;
	class Plugin;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class ProjectManagerListener
	{
	public:
		virtual void OnProjectOpened(ProjectData* data){};
		virtual void OnProjectClosed(){};
		virtual void OnProjectResourcesRefreshed(){};
	};

	class ProjectManager
	{

	private:
		struct ReimportResult
		{
			ResourceID id				= 0;
			TypeID	   tid				= 0;
			bool	   success			= false;
			String	   displayName		= "";
			bool	   isEditorResource = false;
		};

		struct EditorResourceReimportData
		{
			StringID   lastModifiedSID = 0;
			ResourceID id			   = 0;
			TypeID	   tid			   = 0;
			String	   path			   = "";
		};

	public:
		enum class WorkState
		{
			None,
			Working,
			Done
		};
		void Initialize(Editor* editor);
		void Shutdown();
		void PreTick();

		void	   OpenDialogSelectProject();
		void	   OpenDialogCreateProject();
		void	   OpenProject(const String& projectFile);
		void	   SaveProjectChanges();
		ResourceID ConsumeResourceID();

		void LoadGamePlugin(bool notifyError);

		void AddListener(ProjectManagerListener* listener);
		void RemoveListener(ProjectManagerListener* listener);
		void NotifyProjectResourcesRefreshed();

		inline ProjectData* GetProjectData() const
		{
			return m_currentProject;
		}

		TextureAtlasImage* GetThumbnail(ResourceDirectory* dir);
		void			   SetThumbnail(ResourceDirectory* dir, TextureAtlasImage* img);
		void			   ReimportChangedSources(ResourceDirectory* root, LinaGX::Window* requestingWindow);
		void			   CollectReimportResources(ResourceDirectory* dir);
		void			   CollectReimportResourcesRecursively(ResourceDirectory* dir);

	private:
		void OnPressedOpenProject();
		void OnPressedCreateProject();

		void RemoveCurrentProject();
		void CreateEmptyProjectAndOpen(const String& path);
		void VerifyProjectResources(ProjectData* projectData);
		void RemoveDandlingDirectories(ProjectData* projectData, ResourceDirectory* dir);
		void GenerateInitialThumbnails(ProjectData* projectData, ResourceDirectory* dir);

	private:
		static constexpr uint64 REIMPORT_CHECK_TICKS = 30;

		WidgetManager*							m_primaryWidgetManager = nullptr;
		Editor*									m_editor			   = nullptr;
		ProjectData*							m_currentProject	   = nullptr;
		Vector<ProjectManagerListener*>			m_listeners;
		Atomic<bool>							m_thumbnailsWorking = false;
		HashMap<ResourceID, TextureAtlasImage*> m_resourceThumbnails;
		HashMap<ResourceID, TextureAtlasImage*> m_resourceThumbnailsOnFlight;
		HashSet<ResourceID>						m_thumbnailQueue;
		Vector<ReimportResult>					m_reimportResults;
		Vector<ResourceDirectory*>				m_resourcesToReimport;
		uint64									m_lastReimportCheckTicks = 0;
		bool									m_checkReimport			 = false;
		Vector<EditorResourceReimportData>		m_editorResourceReimports;
		Vector<Resource*>						m_editorResourcesToReimport;
		Plugin*									m_gamePlugin			 = nullptr;
		StringID								m_gamePluginLastModified = 0;
		PluginInterface							m_pluginInterface		 = {};
	};
} // namespace Lina::Editor
