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

namespace Lina
{
	class ProjectData;
	class WidgetManager;
	struct ResourceDirectory;
	struct TextureAtlasImage;
	class Widget;
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
		struct ReimportData
		{
			ResourceID id		   = 0;
			TypeID	   tid		   = 0;
			bool	   success	   = false;
			String	   displayName = "";

			bool operator==(const ReimportData& other) const
			{
				return id == other.id;
			}
		};

		struct ReimportDataHash
		{
			std::size_t operator()(const ReimportData& rd) const noexcept
			{
				return std::hash<ResourceID>{}(rd.id);
			}
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

		void	   OpenDialogSelectProject();
		void	   OpenDialogCreateProject();
		void	   OpenProject(const String& projectFile);
		void	   SaveProjectChanges();
		ResourceID ConsumeResourceID();

		void AddListener(ProjectManagerListener* listener);
		void RemoveListener(ProjectManagerListener* listener);
		void NotifyProjectResourcesRefreshed();

		inline ProjectData* GetProjectData() const
		{
			return m_currentProject;
		}

		TextureAtlasImage* GetThumbnail(ResourceDirectory* dir);
		void			   SetThumbnail(ResourceDirectory* dir, TextureAtlasImage* img);
		void			   ReimportChangedSources(ResourceDirectory* root, Widget* requestingWidget);
		void			   ReloadResourceInstances(Resource* res);

	private:
		void OnPressedOpenProject();
		void OnPressedCreateProject();

		void RemoveCurrentProject();
		void CreateEmptyProjectAndOpen(const String& path);
		void VerifyProjectResources(ProjectData* projectData);
		void RemoveDandlingDirectories(ProjectData* projectData, ResourceDirectory* dir);
		void GenerateInitialThumbnails(ProjectData* projectData, ResourceDirectory* dir);

		void ReimportDirectory(ResourceDirectory* c, String& progress);
		void ReimportRecursively(ResourceDirectory* root, String& progress);

	private:
		WidgetManager*							m_primaryWidgetManager = nullptr;
		Editor*									m_editor			   = nullptr;
		ProjectData*							m_currentProject	   = nullptr;
		Vector<ProjectManagerListener*>			m_listeners;
		Atomic<bool>							m_thumbnailsWorking = false;
		HashMap<ResourceID, TextureAtlasImage*> m_resourceThumbnails;
		HashMap<ResourceID, TextureAtlasImage*> m_resourceThumbnailsOnFlight;
		HashSet<ResourceID>						m_thumbnailQueue;
		HashSet<ReimportData, ReimportDataHash> m_reimportQueue;
	};
} // namespace Lina::Editor
