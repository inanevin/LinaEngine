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

#include "Editor/Project/ProjectManager.hpp"
#include "Editor/Graphics/SurfaceRenderer.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Widgets/Popups/ProjectSelector.hpp"
#include "Editor/Widgets/FX/ProgressCircleFill.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Resources/ResourcePipeline.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"

namespace Lina::Editor
{

	void ProjectManager::Initialize(Editor* editor)
	{
		m_editor = editor;

		m_primaryWidgetManager = &editor->GetWindowPanelManager().GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		const EditorSettings& settings = m_editor->GetSettings();

		if (FileSystem::FileOrPathExists(settings.GetLastProjectPath()))
			OpenProject(settings.GetLastProjectPath());
		else
		{

			Vector<CommonWidgets::GenericPopupButton> buttons = {
				{
					.title = Locale::GetStr(LocaleStr::Open),
					.onPressed =
						[this]() {
							const Vector<String> projectPaths = PlatformProcess::OpenDialog({
								.title				   = Locale::GetStr(LocaleStr::OpenExistingProject),
								.primaryButton		   = Locale::GetStr(LocaleStr::Open),
								.extensionsDescription = "",
								.extensions			   = {"linaproject"},
								.mode				   = PlatformProcess::DialogMode::SelectFile,
							});

							if (!projectPaths.empty())
							{
								m_editor->GetWindowPanelManager().UnlockAllForegrounds();
								OpenProject(projectPaths.front());
							}
						},
				},
				{
					.title = Locale::GetStr(LocaleStr::Create),
					.onPressed =
						[this]() {
							const String path = PlatformProcess::SaveDialog({
								.title				   = Locale::GetStr(LocaleStr::CreateNewProject),
								.primaryButton		   = Locale::GetStr(LocaleStr::Create),
								.extensionsDescription = "",
								.extensions			   = {"linaproject"},
							});

							if (!path.empty())
							{
								m_editor->GetWindowPanelManager().UnlockAllForegrounds();
								CreateEmptyProjectAndOpen(path);
							}
						},
				},
			};

			Widget* lock = m_editor->GetWindowPanelManager().LockAllForegrounds(m_primaryWidgetManager->GetRoot()->GetWindow(), Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));

			lock->AddChild(CommonWidgets::BuildGenericPopupWithButtons(lock, Locale::GetStr(LocaleStr::NoProjectFound), buttons));
		}
	}

	void ProjectManager::PreTick()
	{
		if (m_resourceAtlasesNeedUpdate.load(std::memory_order_relaxed))
		{
			m_editor->GetWindowPanelManager().UnlockAllForegrounds();
			m_editor->GetAtlasManager().RefreshPoolAtlases();
			m_resourceAtlasesNeedUpdate.store(false);
			for (ProjectManagerListener* listener : m_listeners)
				listener->OnProjectResourcesRefreshed();
		}
	}

	void ProjectManager::Shutdown()
	{
		RemoveCurrentProject();
	}

	void ProjectManager::OpenDialogCreateProject()
	{
		const String path = PlatformProcess::SaveDialog({
			.title				   = Locale::GetStr(LocaleStr::CreateNewProject),
			.primaryButton		   = Locale::GetStr(LocaleStr::Create),
			.extensionsDescription = "",
			.extensions			   = {"linaproject"},
		});

		if (!path.empty())
		{
			CreateEmptyProjectAndOpen(path);
		}
	}

	void ProjectManager::OpenDialogSelectProject()
	{
		const Vector<String> projectPaths = PlatformProcess::OpenDialog({
			.title				   = Locale::GetStr(LocaleStr::OpenExistingProject),
			.primaryButton		   = Locale::GetStr(LocaleStr::Open),
			.extensionsDescription = "",
			.extensions			   = {"linaproject"},
			.mode				   = PlatformProcess::DialogMode::SelectFile,
		});

		if (!projectPaths.empty())
		{
			OpenProject(projectPaths.front());
		}
	}

	void ProjectManager::SaveProjectChanges()
	{
		m_currentProject->SetDirty(false);
		m_currentProject->SaveToFile();
	}

	ResourceID ProjectManager::ConsumeResourceID()
	{
		const ResourceID id = m_currentProject->ConsumeResourceID();
		SaveProjectChanges();
		return id;
	}

	void ProjectManager::AddListener(ProjectManagerListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void ProjectManager::RemoveListener(ProjectManagerListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](ProjectManagerListener* l) -> bool { return l == listener; }));
	}

	void ProjectManager::CreateEmptyProjectAndOpen(const String& path)
	{
		ProjectData dummy;
		dummy.SetPath(path);
		dummy.SetProjectName(FileSystem::GetFilenameOnlyFromPath(path));
		dummy.SaveToFile();

		const String resCache = dummy.GetResourceDirectory();
		if (FileSystem::FileOrPathExists(resCache))
			FileSystem::DeleteDirectory(resCache);

		OpenProject(path);
	}

	void ProjectManager::RemoveCurrentProject()
	{
		if (m_currentProject == nullptr)
			return;

		RemoveDirectoryThumbnails(&m_currentProject->GetResourceRoot());
		MarkResourceAtlasesNeedUpdate();

		delete m_currentProject;
		m_currentProject = nullptr;

		for (ProjectManagerListener* listener : m_listeners)
			listener->OnProjectClosed();
	}

	void ProjectManager::OpenProject(const String& projectFile)
	{
		RemoveCurrentProject();
		m_currentProject = new ProjectData();
		m_currentProject->SetPath(projectFile);

		// Create resource directory if not existing.
		if (!FileSystem::FileOrPathExists(m_currentProject->GetResourceDirectory()))
			FileSystem::CreateFolderInPath(m_currentProject->GetResourceDirectory());

		// Lock windows & add progress.
		Widget* lock		 = m_editor->GetWindowPanelManager().LockAllForegrounds(m_primaryWidgetManager->GetRoot()->GetWindow(), Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));
		Widget* progress	 = CommonWidgets::BuildGenericPopupProgress(m_primaryWidgetManager->GetRoot(), Locale::GetStr(LocaleStr::CreatingProject), true);
		Text*	progressText = static_cast<Text*>(progress->FindChildWithDebugName("Progress"));
		lock->AddChild(progress);

		Taskflow tf;
		tf.emplace([this, progressText]() {
			auto updateProg = [progressText](const String& txt) {
				progressText->GetProps().text = txt;
				progressText->CalculateTextSize();
			};

			updateProg(Locale::GetStr(LocaleStr::LoadingProjectData));
			m_currentProject->LoadFromFile();

			ResourceDirectory* linaAssets = m_currentProject->GetResourceRoot().GetChildByName(EDITOR_DEF_RESOURCES_FOLDER);

			if (linaAssets == nullptr)
			{
				ResourceDirectory* lina = m_currentProject->GetResourceRoot().CreateChild({
					.name	  = EDITOR_DEF_RESOURCES_FOLDER,
					.isFolder = true,
				});

				const Vector<ResourcePipeline::ResourceImportDef> importDefinitions = {
					{
						.path = EDITOR_FONT_ROBOTO_PATH,
						.id	  = EDITOR_FONT_ROBOTO_ID,
					},
					{
						.path = EDITOR_SHADER_DEFAULT_OBJECT_PATH,
						.id	  = EDITOR_SHADER_DEFAULT_OBJECT_ID,
					},
					{
						.path = EDITOR_SHADER_DEFAULT_SKY_PATH,
						.id	  = EDITOR_SHADER_DEFAULT_SKY_ID,
					},
					{
						.path = EDITOR_SHADER_DEFAULT_LIGHTING_PATH,
						.id	  = EDITOR_SHADER_DEFAULT_LIGHTING_ID,
					},
					{
						.path = EDITOR_MODEL_CUBE_PATH,
						.id	  = EDITOR_MODEL_CUBE_ID,
					},
					{
						.path = EDITOR_MODEL_SPHERE_PATH,
						.id	  = EDITOR_MODEL_SPHERE_ID,
					},
					{
						.path = EDITOR_MODEL_CYLINDER_PATH,
						.id	  = EDITOR_MODEL_CYLINDER_ID,
					},
					{
						.path = EDITOR_MODEL_PLANE_PATH,
						.id	  = EDITOR_MODEL_PLANE_ID,
					},
					{
						.path = EDITOR_MODEL_SKYCUBE_PATH,
						.id	  = EDITOR_MODEL_SKYCUBE_ID,
					},
					{
						.path = EDITOR_TEXTURE_EMPTY_ALBEDO_PATH,
						.id	  = EDITOR_TEXTURE_EMPTY_ALBEDO_ID,
					},
					{
						.path = EDITOR_TEXTURE_EMPTY_NORMAL_PATH,
						.id	  = EDITOR_TEXTURE_EMPTY_NORMAL_ID,
					},
				};

				updateProg(Locale::GetStr(LocaleStr::CreatingCoreResources));
				ResourcePipeline::ImportResources(m_currentProject, lina, importDefinitions, [updateProg](uint32 count, const ResourcePipeline::ResourceImportDef& currentDef, bool isComplete) {
					if (!currentDef.path.empty())
						updateProg(currentDef.path);
				});

				updateProg(EDITOR_SAMPLER_DEFAULT_PATH);
				ResourcePipeline::SaveNewResource(m_currentProject, lina, EDITOR_SAMPLER_DEFAULT_PATH, GetTypeID<TextureSampler>(), EDITOR_SAMPLER_DEFAULT_ID);
			}

			updateProg(Locale::GetStr(LocaleStr::VerifyingProjectResources));
			VerifyProjectResources(m_currentProject);

			updateProg(Locale::GetStr(LocaleStr::GeneratingThumbnails));
			GenerateMissingAtlasImages(&m_currentProject->GetResourceRoot());

			updateProg(Locale::GetStr(LocaleStr::Saving));
			m_currentProject->SaveToFile();

			EditorSettings& settings = m_editor->GetSettings();
			settings.SetLastProjectPath(m_currentProject->GetPath());
			settings.SaveToFile();

			MarkResourceAtlasesNeedUpdate();

			for (ProjectManagerListener* listener : m_listeners)
				listener->OnProjectOpened(m_currentProject);
		});

		m_executor.RunMove(tf);
	}

	void ProjectManager::VerifyProjectResources(ProjectData* projectData)
	{
		// Check if the actual resource file for a ResourceDirectory in the project exists, if not delete the directory.
		ResourceDirectory* root = &projectData->GetResourceRoot();
		VerifyResourceDirectory(projectData, root);

		// Go through all the resource files in the cache directory.
		// If the equivalent resource is not included in/used by the project delete the file to prevent littering.
		const String   resDir = projectData->GetResourceDirectory();
		Vector<String> files;
		FileSystem::GetFilesInDirectory(resDir, files);
		for (const String& file : files)
		{
			const String fileName = FileSystem::GetFilenameOnlyFromPath(file);
			const size_t under	  = fileName.find("_");
			if (under == String::npos)
				continue;
			const String	   resID	= fileName.substr(under + 1, fileName.length() - under);
			const ResourceID   resIDInt = static_cast<ResourceID>(UtilStr::StringToBigInt(resID));
			ResourceDirectory* found	= root->FindResource(resIDInt);
			if (found == nullptr)
				FileSystem::DeleteFileInPath(file);
		}

		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ProjectManager::VerifyResourceDirectory(ProjectData* projectData, ResourceDirectory* dir)
	{
		Vector<ResourceDirectory*> killList;

		for (ResourceDirectory* c : dir->children)
		{
			if (!c->isFolder)
			{
				const String path = projectData->GetResourcePath(c->resourceID);
				if (!FileSystem::FileOrPathExists(path))
				{
					killList.push_back(c);
				}
			}
			else
				VerifyResourceDirectory(projectData, c);
		}

		for (ResourceDirectory* d : killList)
			dir->DestroyChild(d);
	}

	void ProjectManager::GenerateMissingAtlasImages(ResourceDirectory* dir, bool isRecursive)
	{
		if (!dir->thumbnailBuffer.IsEmpty())
		{
			if (dir->_thumbnailAtlasImage == nullptr)
			{
				if (dir->resourceTID == GetTypeID<Font>())
					dir->_thumbnailAtlasImage = m_editor->GetAtlasManager().AddImageToAtlas(dir->thumbnailBuffer.GetRaw(), Vector2ui(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE), LinaGX::Format::R8_UNORM);
				else
					dir->_thumbnailAtlasImage = m_editor->GetAtlasManager().AddImageToAtlas(dir->thumbnailBuffer.GetRaw(), Vector2ui(RESOURCE_THUMBNAIL_SIZE, RESOURCE_THUMBNAIL_SIZE), LinaGX::Format::R8G8B8A8_SRGB);
			}
		}
		else
			dir->_thumbnailAtlasImage = m_editor->GetAtlasManager().GetImageFromAtlas("ProjectIcons"_hs, "FileShaderSmall"_hs);

		for (ResourceDirectory* c : dir->children)
			GenerateMissingAtlasImages(c);
	}

	void ProjectManager::RemoveDirectoryThumbnails(ResourceDirectory* dir)
	{
		if (dir->_thumbnailAtlasImage != nullptr && !dir->thumbnailBuffer.IsEmpty())
			m_editor->GetAtlasManager().RemoveImage(dir->_thumbnailAtlasImage);

		dir->_thumbnailAtlasImage = nullptr;
		for (ResourceDirectory* c : dir->children)
			RemoveDirectoryThumbnails(c);
	}
} // namespace Lina::Editor
