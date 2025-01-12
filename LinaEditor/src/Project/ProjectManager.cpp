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
#include "Editor/Widgets/Popups/NotificationDisplayer.hpp"
#include "Editor/Widgets/EditorRoot.hpp"
#include "Editor/Resources/ResourcePipeline.hpp"
#include "Editor/IO/ThumbnailGenerator.hpp"
#include "Editor/Widgets/Panel/PanelMaterialViewer.hpp"
#include "Editor/World/WorldUtility.hpp"
#include "Editor/Resources/ShaderImport.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Core/System/Plugin.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/Application.hpp"

namespace Lina::Editor
{

	void ProjectManager::Initialize(Editor* editor)
	{
		if (!FileSystem::FileOrPathExists("Resources/Editor/_out/"))
			FileSystem::CreateFolderInPath("Resources/Editor/_out");

		m_editor = editor;
		m_pluginInterface.SetReflectionSystem(&ReflectionSystem::Get());

		m_primaryWidgetManager = &editor->GetWindowPanelManager().GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();

		m_editorResourceReimports = {
			{
				.lastModifiedSID = TO_SID(FileSystem::GetLastModifiedDate(EDITOR_SHADER_WORLD_GRID_PATH)),
				.id				 = EDITOR_SHADER_WORLD_GRID_ID,
				.tid			 = GetTypeID<Shader>(),
				.path			 = EDITOR_SHADER_WORLD_GRID_PATH,
			},
			{
				.lastModifiedSID = TO_SID(FileSystem::GetLastModifiedDate(EDITOR_SHADER_WORLD_OUTLINE_FULLSCREEN_PATH)),
				.id				 = EDITOR_SHADER_WORLD_OUTLINE_FULLSCREEN_ID,
				.tid			 = GetTypeID<Shader>(),
				.path			 = EDITOR_SHADER_WORLD_OUTLINE_FULLSCREEN_PATH,
			},
			{
				.lastModifiedSID = TO_SID(FileSystem::GetLastModifiedDate(EDITOR_SHADER_WORLD_GIZMO_PATH)),
				.id				 = EDITOR_SHADER_WORLD_GIZMO_ID,
				.tid			 = GetTypeID<Shader>(),
				.path			 = EDITOR_SHADER_WORLD_GIZMO_PATH,
			},
		};

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

			Widget* lock = m_editor->GetWindowPanelManager().LockAllForegrounds(m_primaryWidgetManager->GetRoot()->GetWindow(), nullptr);

			lock->AddChild(CommonWidgets::BuildGenericPopupWithButtons(lock, Locale::GetStr(LocaleStr::NoProjectFound), buttons));
		}
	}

	void ProjectManager::Shutdown()
	{
		if (m_gamePlugin)
			PlatformProcess::UnloadPlugin(m_gamePlugin);

		RemoveCurrentProject();
	}

	void ProjectManager::PreTick()
	{
		if (m_currentProject == nullptr)
			return;

		if (m_checkReimport)
		{
			m_lastReimportCheckTicks++;

			if (m_lastReimportCheckTicks > REIMPORT_CHECK_TICKS)
			{
				ReimportChangedSources(&m_currentProject->GetResourceRoot(), m_editor->GetWindowPanelManager().GetMainWindow());

				if (m_gamePlugin)
				{
					const String& path = m_gamePlugin->GetPath();
					if (FileSystem::FileOrPathExists(path))
					{
						const StringID lastModified = TO_SID(FileSystem::GetLastModifiedDate(path));

						if (lastModified != m_gamePluginLastModified)
						{
							m_gamePluginLastModified = lastModified;
							LoadGamePlugin(true);
						}
					}
				}
			}
		}
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

	void ProjectManager::LoadGamePlugin(bool notifyError)
	{
#ifdef LINA_PLATFORM_WINDOWS

		const String path = FileSystem::GetFilePath(m_currentProject->GetPath()) + "GamePlugin/bin/GamePlugin.dll";
#else
        const String path = "";
#endif

		auto notify = [&](bool success) {
			if (success == false && !notifyError)
				return;

			NotificationDisplayer* notificationDisplayer = m_editor->GetWindowPanelManager().GetNotificationDisplayer(m_editor->GetWindowPanelManager().GetMainWindow());
			notificationDisplayer->AddNotification({
				.icon				= success ? NotificationIcon::OK : NotificationIcon::Err,
				.title				= (success ? Locale::GetStr(LocaleStr::LoadGamePluginSuccess) : Locale::GetStr(LocaleStr::LoadGamePluginFail)) + ": " + (m_gamePlugin ? m_gamePlugin->GetPath() : ""),
				.autoDestroySeconds = 5,
			});
		};

		if (!FileSystem::FileOrPathExists(path))
		{
			notify(false);
			return;
		}

		if (m_gamePlugin)
		{
			PlatformProcess::UnloadPlugin(m_gamePlugin);
			m_gamePlugin = nullptr;
		}

		// :( i dont like this. OS file copy operations delay.
		std::chrono::milliseconds d(100);
		std::this_thread::sleep_for(d);

		const String binDirectory = FileSystem::GetFilePath(m_currentProject->GetPath()) + "GamePlugin/bin";
		FileSystem::CopyDirectory(binDirectory, "Resources/Editor/_out/");
		m_gamePlugin			 = PlatformProcess::LoadPlugin("Resources/Editor/_out/bin/GamePlugin.dll", &m_pluginInterface);
		m_gamePluginLastModified = TO_SID(FileSystem::GetLastModifiedDate(path));

		if (m_gamePlugin)
			m_gamePlugin->SetPath(path);

		notify(m_gamePlugin != nullptr);
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
		m_checkReimport = false;
		RemoveCurrentProject();
		m_currentProject = new ProjectData();
		m_currentProject->SetPath(projectFile);

		// Create resource directory if not existing.
		if (!FileSystem::FileOrPathExists(m_currentProject->GetResourceDirectory()))
			FileSystem::CreateFolderInPath(m_currentProject->GetResourceDirectory());

		EditorTask* task   = m_editor->GetTaskManager().CreateTask();
		task->title		   = Locale::GetStr(LocaleStr::CreatingProject);
		task->progressText = Locale::GetStr(LocaleStr::LoadingProjectData);
		task->ownerWindow  = m_editor->GetWindowPanelManager().GetMainWindow();

		task->task = [task, this, projectFile]() {
			const String folder = FileSystem::GetFilePath(projectFile);

			if (!FileSystem::FileOrPathExists(folder + "/GamePlugin"))
			{
				FileSystem::CopyDirectory("Resources/Editor/GamePlugin", folder);
			}
			else
			{
				FileSystem::CopyDirectory("Resources/Editor/GamePlugin/Dependencies", folder + "/GamePlugin/");
				FileSystem::CopyDirectory("Resources/Editor/GamePlugin/CMake", folder + "/GamePlugin/");
			}

			LoadGamePlugin(false);

			task->progressText = Locale::GetStr(LocaleStr::LoadingProjectData);
			m_currentProject->LoadFromFile();

			task->progressText = Locale::GetStr(LocaleStr::VerifyingProjectResources);
			VerifyProjectResources(m_currentProject);

			task->progressText = Locale::GetStr(LocaleStr::CreatingCoreResources);

			const Vector<ResourcePipeline::ResourceImportDef> desiredAssets = {
				{
					.path = EDITOR_FONT_ROBOTO_PATH,
					.id	  = EDITOR_FONT_ROBOTO_ID,
				},
				{
					.path = EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_PATH,
					.id	  = EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_ID,
				},
				{
					.path = EDITOR_SHADER_DEFAULT_TRANSPARENT_SURFACE_PATH,
					.id	  = EDITOR_SHADER_DEFAULT_TRANSPARENT_SURFACE_ID,
				},
				{
					.path = EDITOR_SHADER_DEFAULT_SKY_PATH,
					.id	  = EDITOR_SHADER_DEFAULT_SKY_ID,
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
					.path = EDITOR_MODEL_CAPSULE_PATH,
					.id	  = EDITOR_MODEL_CAPSULE_ID,
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

			ResourceDirectory* root = &m_currentProject->GetResourceRoot();

			// Recreate resources everytime.
			Vector<ResourceDirectory*> engineResources;
			for (ResourceDirectory* c : root->children)
			{
				if (c->userData.directoryType != static_cast<uint32>(ResourceDirectoryType::EngineResource))
					continue;

				engineResources.push_back(c);
				FileSystem::DeleteFileInPath(m_currentProject->GetResourcePath(c->resourceID));
			}

			for (ResourceDirectory* engineRes : engineResources)
				m_currentProject->DestroyResourceDirectory(engineRes);

			Vector<ResourceDirectory*> importedEngineResources = ResourcePipeline::ImportResources(m_currentProject, root, desiredAssets, [task](uint32 count, const ResourcePipeline::ResourceImportDef& currentDef, bool isComplete) {
				if (!currentDef.path.empty())
					task->progressText = currentDef.path;
			});

			for (ResourceDirectory* importedEngineResource : importedEngineResources)
				importedEngineResource->userData.directoryType = static_cast<uint32>(ResourceDirectoryType::EngineResource);

			// Custom sampler.
			if (root->FindResourceDirectory(EDITOR_SAMPLER_DEFAULT_ID) == nullptr)
			{
				task->progressText	   = EDITOR_SAMPLER_DEFAULT_PATH;
				ResourceDirectory* dir = ResourcePipeline::SaveNewResource(m_currentProject, root, EDITOR_SAMPLER_DEFAULT_PATH, GetTypeID<TextureSampler>(), EDITOR_SAMPLER_DEFAULT_ID);

				if (dir)
					dir->userData.directoryType = static_cast<uint32>(ResourceDirectoryType::EngineResource);
			}

			if (root->FindResourceDirectory(EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID) == nullptr)
			{
				task->progressText	   = EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_PATH;
				ResourceDirectory* dir = ResourcePipeline::SaveNewResource(m_currentProject, root, EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_PATH, GetTypeID<Material>(), EDITOR_MATERIAL_DEFAULT_OPAQUE_OBJECT_ID, EDITOR_SHADER_DEFAULT_OPAQUE_SURFACE_ID);

				if (dir)
					dir->userData.directoryType = static_cast<uint32>(ResourceDirectoryType::EngineResource);
			}

			// Custom material
			if (root->FindResourceDirectory(EDITOR_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_ID) == nullptr)
			{
				task->progressText = EDITOR_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_PATH;
				ResourceDirectory* dir =
					ResourcePipeline::SaveNewResource(m_currentProject, root, EDITOR_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_PATH, GetTypeID<Material>(), EDITOR_MATERIAL_DEFAULT_TRANSPARENT_OBJECT_ID, EDITOR_SHADER_DEFAULT_TRANSPARENT_SURFACE_ID);

				if (dir)
					dir->userData.directoryType = static_cast<uint32>(ResourceDirectoryType::EngineResource);
			}

			if (root->FindResourceDirectory(EDITOR_MATERIAL_DEFAULT_SKY_ID) == nullptr)
			{
				task->progressText	   = EDITOR_MATERIAL_DEFAULT_SKY_PATH;
				ResourceDirectory* dir = ResourcePipeline::SaveNewResource(m_currentProject, root, EDITOR_MATERIAL_DEFAULT_SKY_PATH, GetTypeID<Material>(), EDITOR_MATERIAL_DEFAULT_SKY_ID, EDITOR_SHADER_DEFAULT_SKY_ID);

				if (dir)
					dir->userData.directoryType = static_cast<uint32>(ResourceDirectoryType::EngineResource);
			}

			task->progressText = Locale::GetStr(LocaleStr::GeneratingThumbnails);
			GenerateInitialThumbnails(m_currentProject, &m_currentProject->GetResourceRoot());

			task->progressText = Locale::GetStr(LocaleStr::Saving);
			m_currentProject->SaveToFile();

			EditorSettings& settings = m_editor->GetSettings();
			settings.SetLastProjectPath(m_currentProject->GetPath());
			settings.SaveToFile();
		};

		task->onComplete = [task, this]() {
			m_editor->GetAtlasManager().RefreshAtlasPool();
			m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

			for (ProjectManagerListener* listener : m_listeners)
				listener->OnProjectOpened(m_currentProject);
			m_editor->GetSettings().GetLayout().ApplyStoredLayout();

			ReimportChangedSources(&m_currentProject->GetResourceRoot(), m_editor->GetWindowPanelManager().GetMainWindow());
			m_checkReimport = true;
		};

		m_editor->GetTaskManager().AddTask(task);
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
			m_editor->GetProjectManager().GetProjectData()->DestroyResourceDirectory(d);
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

	void ProjectManager::SetThumbnail(ResourceDirectory* dir, TextureAtlasImage* img)
	{
		TextureAtlasImage* ex = m_resourceThumbnails[dir->resourceID];
		if (ex != nullptr)
			m_editor->GetAtlasManager().RemoveImage(ex);

		m_resourceThumbnails[dir->resourceID] = img;
		m_editor->GetAtlasManager().RefreshAtlasPool();
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

	void ProjectManager::CollectReimportResources(ResourceDirectory* dir)
	{
		if (dir->userData.directoryType != static_cast<uint32>(ResourceDirectoryType::Default))
			return;

		if (dir->resourceType != ResourceType::ExternalSource)
			return;

		const String path = FileSystem::GetFilePath(m_currentProject->GetPath()) + dir->sourcePathRelativeToProject;

		if (!FileSystem::FileOrPathExists(path))
			return;

		const StringID lastModifiedSID = TO_SID(FileSystem::GetLastModifiedDate(path));

		if (lastModifiedSID == dir->lastModifiedSID)
			return;

		dir->lastModifiedSID = lastModifiedSID;
		m_resourcesToReimport.push_back(dir);
	}

	void ProjectManager::CollectReimportResourcesRecursively(ResourceDirectory* dir)
	{
		for (ResourceDirectory* c : dir->children)
		{
			if (c->isFolder)
			{
				CollectReimportResourcesRecursively(c);
				continue;
			}

			CollectReimportResources(c);
		}
	}

	void ProjectManager::ReimportChangedSources(ResourceDirectory* root, LinaGX::Window* window)
	{
		m_lastReimportCheckTicks = 0;

		m_reimportResults.clear();
		m_resourcesToReimport.clear();
		m_editorResourcesToReimport.clear();

		CollectReimportResourcesRecursively(root);

		for (EditorResourceReimportData& data : m_editorResourceReimports)
		{
			const String   lastModified	   = FileSystem::GetLastModifiedDate(data.path);
			const StringID lastModifiedSID = TO_SID(lastModified);
			if (lastModifiedSID == data.lastModifiedSID)
				continue;
			Resource* res = m_editor->GetApp()->GetResourceManager().GetIfExists(data.tid, data.id);
			res->SetPath(data.path);
			m_editorResourcesToReimport.push_back(res);
			data.lastModifiedSID = lastModifiedSID;
		}

		if (m_resourcesToReimport.empty() && m_editorResourcesToReimport.empty())
			return;

		EditorTask* task = m_editor->GetTaskManager().CreateTask();

		task->ownerWindow  = window;
		task->title		   = Locale::GetStr(LocaleStr::Reimporting);
		task->progressText = Locale::GetStr(LocaleStr::Working);
		task->task		   = [this, root, task]() {
			for (ResourceDirectory* dir : m_resourcesToReimport)
			{
				task->progressText = dir->name;

				MetaType*	 meta	 = ReflectionSystem::Get().Resolve(dir->resourceTID);
				Resource*	 res	 = static_cast<Resource*>(meta->GetFunction<void*()>("Allocate"_hs)());
				const String resPath = m_currentProject->GetResourcePath(dir->resourceID);

				// Load all data first.
				IStream stream = Serialization::LoadFromFile(resPath.c_str());
				if (!stream.Empty())
					res->LoadFromStream(stream);
				stream.Destroy();

				const String path = FileSystem::GetFilePath(m_currentProject->GetPath()) + dir->sourcePathRelativeToProject;

				// Now reload from file & save if success.
				bool success = res->LoadFromFile(path);
				if (success)
					res->SaveToFileAsBinary(resPath);

				meta->GetFunction<void(void*)>("Deallocate"_hs)(res);

				m_reimportResults.push_back({
							.id			 = dir->resourceID,
							.tid		 = dir->resourceTID,
							.success	 = success,
							.displayName = dir->name,
				});
			}

			for (Resource* res : m_editorResourcesToReimport)
			{
				const bool success = res->LoadFromFile(res->GetPath());

				m_reimportResults.push_back({
							.id				  = res->GetID(),
							.tid			  = res->GetTID(),
							.success		  = success,
							.displayName	  = res->GetName(),
							.isEditorResource = true,
				});
			}
		};

		task->onComplete = [this]() {
			SaveProjectChanges();

			WidgetManager&		   wm					 = m_editor->GetWindowPanelManager().GetSurfaceRenderer(LINA_MAIN_SWAPCHAIN)->GetWidgetManager();
			NotificationDisplayer* notificationDisplayer = m_editor->GetWindowPanelManager().GetNotificationDisplayer(m_editor->GetWindowPanelManager().GetMainWindow());

			HashSet<Resource*> toReload;

			auto notify = [&](bool success, const String& name) {
				notificationDisplayer->AddNotification({
					.icon				= success ? NotificationIcon::OK : NotificationIcon::Err,
					.title				= (success ? Locale::GetStr(LocaleStr::ReimportedResource) : Locale::GetStr(LocaleStr::FailedReimportingResource)) + " : " + name,
					.autoDestroySeconds = 5,
				});
			};

			for (const ReimportResult& reimport : m_reimportResults)
			{
				if (!reimport.success)
				{
					notify(false, reimport.displayName);
					continue;
				}

				Resource* res = m_editor->GetApp()->GetResourceManager().GetIfExists(reimport.tid, reimport.id);

				if (res == nullptr)
					continue;

				if (reimport.isEditorResource)
				{
					toReload.insert(res);
					notify(true, reimport.displayName);
					continue;
				}

				IStream stream = Serialization::LoadFromFile(m_currentProject->GetResourcePath(res->GetID()).c_str());
				if (stream.Empty())
				{
					notify(false, reimport.displayName);
					continue;
				}

				res->LoadFromStream(stream);
				toReload.insert(res);

				if (res->GetTID() == GetTypeID<Shader>())
				{
					m_editor->GetApp()->GetResourceManager().GetCache<Material>()->View([&](Material* mat, uint32 index) {
						if (mat->GetShader() == res->GetID())
							mat->SetShader(static_cast<Shader*>(res));

						return false;
					});
				}

				Vector<PanelResourceViewer*> panels = m_editor->GetWindowPanelManager().FindResourceViewers(res->GetID());
				for (PanelResourceViewer* panel : panels)
				{
					panel->UpdateResourceProperties();
					panel->RebuildContents();
				}

				notify(true, reimport.displayName);
			}

			m_editor->GetApp()->JoinRender();
			m_editor->GetApp()->GetResourceManager().ReloadResourceHW(toReload);

			// RefreshThumbnails();

			m_reimportResults.clear();
			m_resourcesToReimport.clear();
		};
		m_editor->GetTaskManager().AddTask(task);
	}

} // namespace Lina::Editor
