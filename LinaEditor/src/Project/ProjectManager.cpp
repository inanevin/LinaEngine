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
#include "Editor/Widgets/FX/ProgressCircleFill.hpp"
#include "Editor/EditorLocale.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Resources/ResourcePipeline.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Application.hpp"

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
			const Vector<CommonWidgets::GenericPopupButton> buttons = {
				{
					.title	   = Locale::GetStr(LocaleStr::Open),
					.onPressed = BIND(&ProjectManager::OnPressedOpenProject, this),
				},
				{
					.title	   = Locale::GetStr(LocaleStr::Create),
					.onPressed = BIND(&ProjectManager::OnPressedCreateProject, this),
				},
			};

			Widget* lock = m_editor->GetWindowPanelManager().LockAllForegrounds(m_primaryWidgetManager->GetRoot()->GetWindow(), Locale::GetStr(LocaleStr::WorkInProgressInAnotherWindow));

			lock->AddChild(CommonWidgets::BuildGenericPopupWithButtons(lock, Locale::GetStr(LocaleStr::NoProjectFound), buttons));
		}
	}

	void ProjectManager::PreTick()
	{
		TaskRunner::Poll();

		if (m_frameCtrThumbnails > 60)
		{
			m_frameCtrThumbnails = 0;
			HandleThumbnailRequests();
		}
		m_frameCtrThumbnails++;

		if (m_frameCtrThumbnailsClean > 6000)
		{
			m_frameCtrThumbnailsClean = 0;
			HandleHandleRemoveUnusedThumbnails();
		}
		m_frameCtrThumbnailsClean++;
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

		if (!projectPaths.empty() && !projectPaths.front().empty())
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

		TaskRunner::AddTask(
			[this, progressText]() {
				auto updateProg = [progressText](const String& txt) {
					progressText->GetProps().text = txt;
					progressText->CalculateTextSize();
				};

				updateProg(Locale::GetStr(LocaleStr::LoadingProjectData));
				m_currentProject->LoadFromFile();

				updateProg(Locale::GetStr(LocaleStr::VerifyingProjectResources));
				VerifyProjectResources(m_currentProject);

				updateProg(Locale::GetStr(LocaleStr::CreatingCoreResources));

				const Vector<ResourcePipeline::ResourceImportDef> desiredAssets = {
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
						.path = EDITOR_MODEL_SKYSPHERE_PATH,
						.id	  = EDITOR_MODEL_SKYSPHERE_ID,
					},
					{
						.path = EDITOR_TEXTURE_EMPTY_ALBEDO_PATH,
						.id	  = EDITOR_TEXTURE_EMPTY_ALBEDO_ID,
					},
					{
						.path = EDITOR_TEXTURE_EMPTY_NORMAL_PATH,
						.id	  = EDITOR_TEXTURE_EMPTY_NORMAL_ID,
					},
					{
						.path = EDITOR_TEXTURE_EMPTY_AO_PATH,
						.id	  = EDITOR_TEXTURE_EMPTY_AO_ID,
					},
					{
						.path = EDITOR_TEXTURE_EMPTY_METALLIC_ROUGHNESS_PATH,
						.id	  = EDITOR_TEXTURE_EMPTY_METALLIC_ROUGHNESS_ID,
					},
					{
						.path = EDITOR_TEXTURE_EMPTY_EMISSIVE_PATH,
						.id	  = EDITOR_TEXTURE_EMPTY_EMISSIVE_ID,
					},
				};

				Vector<ResourcePipeline::ResourceImportDef> importDefinitions;
				ResourceDirectory*							linaAssets = m_currentProject->GetResourceRoot().GetChildByName(EDITOR_DEF_RESOURCES_FOLDER);
				if (linaAssets == nullptr)
				{
					importDefinitions = desiredAssets;
					linaAssets		  = m_currentProject->GetResourceRoot().CreateChild({
							   .name	 = EDITOR_DEF_RESOURCES_FOLDER,
							   .isFolder = true,
					   });
				}
				else
				{
					for (const ResourcePipeline::ResourceImportDef& def : desiredAssets)
					{
						if (linaAssets->FindResourceDirectory(def.id) == nullptr)
							importDefinitions.push_back(def);
					}
				}

				if (!importDefinitions.empty())
				{
					ResourcePipeline::ImportResources(m_currentProject, linaAssets, importDefinitions, [updateProg](uint32 count, const ResourcePipeline::ResourceImportDef& currentDef, bool isComplete) {
						if (!currentDef.path.empty())
							updateProg(currentDef.path);
					});
				}

				// Custom sampler.
				if (linaAssets->FindResourceDirectory(EDITOR_SAMPLER_DEFAULT_ID) == nullptr)
				{
					updateProg(EDITOR_SAMPLER_DEFAULT_PATH);
					ResourcePipeline::SaveNewResource(m_currentProject, linaAssets, EDITOR_SAMPLER_DEFAULT_PATH, GetTypeID<TextureSampler>(), EDITOR_SAMPLER_DEFAULT_ID);
				}

				// Custom material
				if (linaAssets->FindResourceDirectory(EDITOR_MATERIAL_DEFAULT_OBJ_ID) == nullptr)
				{
					updateProg(EDITOR_MATERIAL_DEFAULT_OBJ_PATH);
					ResourcePipeline::SaveNewResource(m_currentProject, linaAssets, EDITOR_MATERIAL_DEFAULT_OBJ_PATH, GetTypeID<Material>(), EDITOR_MATERIAL_DEFAULT_OBJ_ID, EDITOR_SHADER_DEFAULT_OBJECT_ID);
				}

				if (linaAssets->FindResourceDirectory(EDITOR_MATERIAL_DEFAULT_SKY_ID) == nullptr)
				{
					updateProg(EDITOR_MATERIAL_DEFAULT_SKY_PATH);
					ResourcePipeline::SaveNewResource(m_currentProject, linaAssets, EDITOR_MATERIAL_DEFAULT_SKY_PATH, GetTypeID<Material>(), EDITOR_MATERIAL_DEFAULT_SKY_ID, EDITOR_SHADER_DEFAULT_SKY_ID);
				}

				if (linaAssets->FindResourceDirectory(EDITOR_MATERIAL_DEFAULT_LIGHTING_ID) == nullptr)
				{
					updateProg(EDITOR_MATERIAL_DEFAULT_LIGHTING_PATH);
					ResourcePipeline::SaveNewResource(m_currentProject, linaAssets, EDITOR_MATERIAL_DEFAULT_LIGHTING_PATH, GetTypeID<Material>(), EDITOR_MATERIAL_DEFAULT_LIGHTING_ID, EDITOR_SHADER_DEFAULT_LIGHTING_ID);
				}

				updateProg(Locale::GetStr(LocaleStr::GeneratingThumbnails));
				GenerateInitialThumbnails(m_currentProject, &m_currentProject->GetResourceRoot());

				updateProg(Locale::GetStr(LocaleStr::Saving));
				m_currentProject->SaveToFile();

				EditorSettings& settings = m_editor->GetSettings();
				settings.SetLastProjectPath(m_currentProject->GetPath());
				settings.SaveToFile();
			},
			[this]() {
				m_editor->GetWindowPanelManager().UnlockAllForegrounds();
				Application::GetLGX()->Join();
				m_editor->GetAtlasManager().RefreshPoolAtlases();
				for (ProjectManagerListener* listener : m_listeners)
					listener->OnProjectOpened(m_currentProject);
			});
	}

	void ProjectManager::VerifyProjectResources(ProjectData* projectData)
	{
		// Check if the actual resource file for a ResourceDirectory in the project exists, if not delete the directory.
		ResourceDirectory* root = &projectData->GetResourceRoot();
		RemoveDandlingDirectories(projectData, root);

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
			ResourceDirectory* found	= root->FindResourceDirectory(resIDInt);
			if (found == nullptr)
				FileSystem::DeleteFileInPath(file);
		}

		m_editor->GetProjectManager().SaveProjectChanges();
	}

	void ProjectManager::RemoveDandlingDirectories(ProjectData* projectData, ResourceDirectory* dir)
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
				RemoveDandlingDirectories(projectData, c);
		}

		for (ResourceDirectory* d : killList)
			dir->DestroyChild(d);
	}

	void ProjectManager::GenerateInitialThumbnails(ProjectData* projectData, ResourceDirectory* dir)
	{
		for (ResourceDirectory* c : dir->children)
			GenerateInitialThumbnails(projectData, c);

		if (dir->isFolder)
			return;

		m_resourceThumbnails[dir->resourceID] = ThumbnailGenerator::GenerateThumbnail(projectData, dir->resourceID, dir->resourceTID, m_editor->GetAtlasManager());
	}

	void ProjectManager::NotifyProjectResourcesRefreshed()
	{
		for (ProjectManagerListener* listener : m_listeners)
			listener->OnProjectResourcesRefreshed();
	}

	TextureAtlasImage* ProjectManager::GetThumbnail(ResourceDirectory* dir)
	{
		return m_resourceThumbnails[dir->resourceID];
	}

	void ProjectManager::InvalidateThumbnail(ResourceDirectory* dir)
	{
		m_resourceThumbnails[dir->resourceID] = nullptr;
	}

	void ProjectManager::AddToThumbnailQueue(ResourceID id)
	{
		m_thumbnailQueue.insert(id);
	}

	void ProjectManager::OnPressedOpenProject()
	{
		const Vector<String> projectPaths = PlatformProcess::OpenDialog({
			.title				   = Locale::GetStr(LocaleStr::OpenExistingProject),
			.primaryButton		   = Locale::GetStr(LocaleStr::Open),
			.extensionsDescription = "",
			.extensions			   = {"linaproject"},
			.mode				   = PlatformProcess::DialogMode::SelectFile,
		});

		if (!projectPaths.empty() && !projectPaths.front().empty())
		{
			m_editor->GetWindowPanelManager().UnlockAllForegrounds();
			OpenProject(projectPaths.front());
		}
	}

	void ProjectManager::OnPressedCreateProject()
	{
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
	}

	void ProjectManager::HandleThumbnailRequests()
	{
		if (m_thumbnailQueue.empty())
			return;

		if (m_thumbnailsWorking.load())
			return;

		// Convert thumbnail queue to resource id - type id pairs.
		Vector<Pair<ResourceID, TypeID>> idPairs;
		for (ResourceID id : m_thumbnailQueue)
		{
			ResourceDirectory* dir = m_currentProject->GetResourceRoot().FindResourceDirectory(id);
			if (dir == nullptr || dir->isFolder)
				continue;
			idPairs.push_back(linatl::make_pair(id, dir->resourceTID));
		}
		m_thumbnailQueue.clear();

		if (idPairs.empty())
			return;

		m_thumbnailsWorking.store(true);

		TaskRunner::AddTask(
			[this, idPairs]() {
				// Generate
				m_resourceThumbnailsOnFlight.clear();
				for (const Pair<ResourceID, TypeID>& pair : idPairs)
					m_resourceThumbnailsOnFlight[pair.first] = ThumbnailGenerator::GenerateThumbnail(m_currentProject, pair.first, pair.second, m_editor->GetAtlasManager());
			},
			[this]() {
				// Remove & replace if thumbnail already exists, add new ones.
				for (auto [id, atlasImg] : m_resourceThumbnailsOnFlight)
				{
					auto it = m_resourceThumbnails.find(id);
					if (it != m_resourceThumbnails.end())
						m_editor->GetAtlasManager().RemoveImage(it->second);
					m_resourceThumbnails[id] = atlasImg;
				}

				Application::GetLGX()->Join();
				m_editor->GetAtlasManager().RefreshPoolAtlases();
				NotifyProjectResourcesRefreshed();
				m_thumbnailsWorking.store(false);
			});
	}

	void ProjectManager::HandleHandleRemoveUnusedThumbnails()
	{
		if (m_thumbnailsWorking.load())
			return;

		Vector<ResourceID> deadlist;

		for (auto [id, atlasImg] : m_resourceThumbnails)
		{
			ResourceDirectory* dir = m_currentProject->GetResourceRoot().FindResourceDirectory(id);
			if (dir != nullptr)
				continue;

			m_editor->GetAtlasManager().RemoveImage(atlasImg);
			deadlist.push_back(id);
		}

		if (!deadlist.empty())
		{
			for (ResourceID id : deadlist)
				m_resourceThumbnails.erase(m_resourceThumbnails.find(id));

			Application::GetLGX()->Join();
			m_editor->GetAtlasManager().RefreshPoolAtlases();
			NotifyProjectResourcesRefreshed();
		}
	}

} // namespace Lina::Editor
